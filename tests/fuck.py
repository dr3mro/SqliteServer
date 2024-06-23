import requests
import concurrent.futures
import time

# Global variables for statistics
failed_requests_count = 0
successful_requests_count = 0
start_time = None

# Function to send GET request to the API endpoint
def send_request(url):
    global failed_requests_count, successful_requests_count
    try:
        with requests.get(url) as response:
            if response.status_code == 200:
                successful_requests_count += 1
            else:
                failed_requests_count += 1
    except requests.RequestException as e:
        failed_requests_count += 1

# Main function
def main():
    global start_time

    # Endpoint URL using 127.0.0.1
    endpoint = "http://127.0.0.1:8080/api_v1/read_patient_basic_information/100003"

    # Number of concurrent requests
    num_requests = 4000

    # Start time
    start_time = time.time()

    # Create a ThreadPoolExecutor with max_workers=num_requests
    with concurrent.futures.ThreadPoolExecutor(max_workers=num_requests) as executor:
        # Schedule the send_request function to be executed concurrently
        # 1000 times with different URLs (same endpoint)
        executor.map(send_request, [endpoint] * num_requests)

    # Calculate elapsed time
    elapsed_time = time.time() - start_time

    # Calculate requests per second (RPS)
    if elapsed_time > 0:
        rps = (successful_requests_count + failed_requests_count) / elapsed_time
    else:
        rps = 0

    # Print statistics
    print(f"Elapsed Time: {elapsed_time:.2f} seconds")
    print(f"Total Requests: {num_requests}")
    print(f"Successful Requests: {successful_requests_count}")
    print(f"Failed Requests: {failed_requests_count}")
    print(f"Requests per Second (RPS): {rps:.2f}")

if __name__ == "__main__":
    main()
