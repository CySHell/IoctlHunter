#include "Jsonize.h"


extern IOCTL_GLOBALS gIoctlGlobals;


HRESULT InitJsonQueue() {
	InitQueue(&gIoctlGlobals.gJsonQueue);
	gIoctlGlobals.gRootJson = json_object_new_array();
	return S_OK;
}

static int json_hex_serializer(struct json_object* jobj,
	printbuf* pb,
	int level,
	int flags) {
	if (!jobj) return -1;

	uint64_t val = json_object_get_uint64(jobj);
	UCHAR buf[0x100] = { 0 };
	snprintf(buf, sizeof(buf), "\"0x%llX\"", val);
	printbuf_memappend_fast(pb, buf, (int)strlen(buf));

	return 0;
}

/**
 * Sets the custom serializer for uint64_t objects so they print in hex format instead of decimal
 */
json_object* set_uint64_hex_serializer(ULONG64 Value) {
	json_object* jobj = json_object_new_uint64(Value);

	json_object_set_serializer(jobj, json_hex_serializer, NULL, NULL);

	return jobj;
}

HRESULT CommitJson(PIOCTL_BREAKPOINT_INFO BpInfo) {
	HRESULT hr = S_OK;

	LogIoctl("%s\n", json_object_to_json_string_ext(BpInfo->rootJson, JSON_C_TO_STRING_PLAIN));
	EnqueueJsonObj(&gIoctlGlobals.gJsonQueue, BpInfo->rootJson);

	return hr;
}


HANDLE consumerThread;

// Consumer thread function
DWORD WINAPI ConsumerThread(LPVOID param) {
    static ULONG64 lastSaveTime = 0;

    CHAR jsonOutputFile[MAX_PATH];
    CHAR* userProfilePath = GetUserProfilePath();
    snprintf(jsonOutputFile, sizeof(jsonOutputFile), "%s\\%s", userProfilePath, JSON_OUTPUT_FILE_NAME);

    while (1) {
        json_object* json = DequeueJsonObj(&gIoctlGlobals.gJsonQueue);

        // Merge JSON object into root JSON
        AcquireSRWLockExclusive(&gIoctlGlobals.gJsonQueue.lock);
        json_object_array_add(gIoctlGlobals.gRootJson, json);
        ReleaseSRWLockExclusive(&gIoctlGlobals.gJsonQueue.lock);

        ULONG64 currentTime = GetTickCount64();
        if (currentTime - lastSaveTime > SAVE_INTERVAL_MS) {
            AcquireSRWLockExclusive(&gIoctlGlobals.gJsonQueue.lock);
            json_object_to_file(jsonOutputFile, gIoctlGlobals.gRootJson);
            lastSaveTime = currentTime;
            ReleaseSRWLockExclusive(&gIoctlGlobals.gJsonQueue.lock);
        }
    }
    return 0;
}


// Start consumer thread
void StartJsonQueueConsumer() {
    consumerThread = CreateThread(NULL, 0, ConsumerThread, NULL, 0, NULL);
}

// Initialize the queue
void InitQueue(JsonQueue* queue) {
    queue->head = queue->tail = NULL;
    InitializeSRWLock(&queue->lock);
    InitializeConditionVariable(&queue->not_empty);
    StartJsonQueueConsumer();
}

// Enqueue JSON object (Producer)
void EnqueueJsonObj(JsonQueue* queue, json_object* json) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    newNode->json = json;
    newNode->next = NULL;

    AcquireSRWLockExclusive(&queue->lock);
    if (!queue->head) {
        queue->head = queue->tail = newNode;
    }
    else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
    WakeConditionVariable(&queue->not_empty);
    ReleaseSRWLockExclusive(&queue->lock);
}

// Dequeue JSON object (Consumer)
json_object* DequeueJsonObj(JsonQueue* queue) {
    AcquireSRWLockExclusive(&queue->lock);
    while (!queue->head) {
        SleepConditionVariableSRW(&queue->not_empty, &queue->lock, INFINITE, 0);
    }
    QueueNode* node = queue->head;
    queue->head = node->next;
    if (!queue->head) queue->tail = NULL;
    ReleaseSRWLockExclusive(&queue->lock);

    json_object* json = node->json;
    free(node);
    return json;
}



// Cleanup queue
void CleanupQueue(JsonQueue* queue) {
    while (queue->head) {
        QueueNode* temp = queue->head;
        queue->head = queue->head->next;
        json_object_put(temp->json);
        free(temp);
    }
}
