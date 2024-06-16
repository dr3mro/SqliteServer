import requests
import json
import random
import string
from concurrent.futures import ThreadPoolExecutor

# Define the base URL and headers
post_url = "http://localhost:18080/post"
get_url_template = "http://localhost:18080/get/{}"
headers = {
    "Accept": "application/json",
    "Content-type": "application/json"
}

# Function to generate a random string
def generate_random_string(length=8):
    letters = string.ascii_letters
    return ''.join(random.choice(letters) for _ in range(length))

def post_request(id, value):
    data = {
        "id": id,
        "value": value
    }
    response = requests.post(post_url, headers=headers, data=json.dumps(data))
    return response.status_code, response.json() if response.status_code == 200 else response.text

def get_request(id):
    url = get_url_template.format(id)
    response = requests.get(url)
    return response.status_code, response.json() if response.status_code == 200 else response.text

def perform_requests(id):
    # Generate a random value string
    random_value = generate_random_string()

    # Perform the POST request
    post_status, post_response = post_request(id, random_value)
    print(f"POST ID: {id}, Status: {post_status}, Response: {post_response}")

    # Perform the GET request
    get_status, get_response = get_request(id)
    print(f"GET ID: {id}, Status: {get_status}, Response: {get_response}")

# Number of threads (corresponding to IDs from 1 to 100)
num_threads = 4

# Use ThreadPoolExecutor to execute requests in parallel
with ThreadPoolExecutor(max_workers=num_threads) as executor:
    futures = [executor.submit(perform_requests, id) for id in range(1, num_threads + 1)]

# Wait for all threads to complete
for future in futures:
    future.result()