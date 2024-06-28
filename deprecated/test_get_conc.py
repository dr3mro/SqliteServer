import concurrent.futures
import requests
import time

# Define the base URL of the API
base_url = "http://localhost:8080/get/"

# Function to send a GET request to a specific endpoint
def send_request(endpoint):
    try:
        response = requests.get(endpoint)
        return response.status_code, response.text, endpoint
    except requests.RequestException as e:
        return None, str(e), endpoint

# Main function to overload the API
def overload_api():
    # Create a list of endpoints to hit
    endpoints = [f"{base_url}{i}" for i in range(1, 1001)]

    # Record the start time
    start_time = time.time()

    # Use ThreadPoolExecutor to send 500 concurrent requests
    with concurrent.futures.ThreadPoolExecutor(max_workers=500) as executor:
        # Map the send_request function to the endpoints
        results = list(executor.map(send_request, endpoints))

        # Record the end time
        end_time = time.time()

        # Calculate the total execution time
        total_time = end_time - start_time

        # Print the results
        for status_code, content, endpoint in results:
            if status_code:
                print(f"Request to {endpoint} returned status code {status_code} with content: {content}")
            else:
                print(f"Request to {endpoint} failed with error: {content}")

        # Print the total execution time
        print(f"Total execution time: {total_time:.2f} seconds")

if __name__ == "__main__":
    overload_api()
