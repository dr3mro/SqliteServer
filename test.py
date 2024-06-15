import requests
import threading
import random
import string

# Target API endpoint
post_url = 'http://localhost:18080/post'

# Number of requests to send
num_requests = 100  # Adjust the number as needed

# Function to generate a random string
def generate_random_string(length):
    letters = string.ascii_letters
    return ''.join(random.choice(letters) for _ in range(length))

# Function to send POST requests with payload (id and value)
def send_post_request():
    try:
        # Generate random id and value
        payload = {"id": random.randint(1, 1000), "value": generate_random_string(10)}
        headers = {"Accept": "application/json", "Content-type": "application/json"}

        # Send POST request with payload
        response = requests.post(post_url, headers=headers, json=payload)

        # Debug print the request details
        print(f"POST Request URL: {post_url}")
        print(f"POST Request Payload: {payload}")
        print(f"POST Request Status Code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Exception occurred during POST request: {e}")

# Function to execute POST requests sequentially
def execute_post_requests():
    print("Executing POST requests sequentially...")
    for _ in range(num_requests):
        send_post_request()
    print("All POST requests sent.")

# Execute POST requests sequentially
execute_post_requests()

print("All operations completed.")
