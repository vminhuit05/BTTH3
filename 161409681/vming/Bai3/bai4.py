import multiprocessing
import random
import time

# Buffer size
BUFFER_SIZE = 10
STOP_THRESHOLD = 100

# Shared memory buffer and index tracking
buffer = multiprocessing.Array('i', BUFFER_SIZE)  # Bounded buffer of 10 integers
write_index = multiprocessing.Value('i', 0)
read_index = multiprocessing.Value('i', 0)

# Semaphore for synchronization
empty = multiprocessing.Semaphore(BUFFER_SIZE)  # Starts with 10 "empty" slots
full = multiprocessing.Semaphore(0)             # Starts with 0 "full" slots
buffer_lock = multiprocessing.Lock()

def producer():
    while True:
        # Create a random integer in range [10, 20]
        item = random.randint(10, 20)

        # Wait for an empty slot
        empty.acquire()

        # Write the item to the buffer
        with buffer_lock:
            buffer[write_index.value] = item
            print(f"Producer: Produced {item} at index {write_index.value}")
            write_index.value = (write_index.value + 1) % BUFFER_SIZE  # Move write index

        # Signal that there is a full slot
        full.release()

        # Small delay for demonstration
        time.sleep(0.5)

def consumer():
    total = 0
    while total <= STOP_THRESHOLD:
        # Wait for a full slot
        full.acquire()

        # Read item from the buffer
        with buffer_lock:
            item = buffer[read_index.value]
            print(f"Consumer: Consumed {item} at index {read_index.value}")
            total += item
            print(f"Consumer: Total = {total}")
            read_index.value = (read_index.value + 1) % BUFFER_SIZE  # Move read index

        # Signal that there is an empty slot
        empty.release()

        # Check if total exceeds the threshold
        if total > STOP_THRESHOLD:
            print("Consumer: Total exceeded threshold, stopping.")
            break

        # Small delay for demonstration
        time.sleep(0.5)

if __name__ == '__main__':
    # Create processes for producer and consumer
    p = multiprocessing.Process(target=producer)
    c = multiprocessing.Process(target=consumer)

    # Start both processes
    p.start()
    c.start()

    # Wait for the consumer to finish
    c.join()

    # Terminate the producer since the consumer has stopped
    p.terminate()
    print("Producer and Consumer have stopped.")
