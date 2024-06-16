import requests
import json
import random
import string
from concurrent.futures import ThreadPoolExecutor

# Function to generate a random string of 100 characters
def generate_random_string(length=100):
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for i in range(length))

# Function to send a POST request
def send_post_request(id):
    url = "http://localhost:18080/post"
    headers = {
        "Accept": "application/json",
        "Content-type": "application/json"
    }
    data = {
        "id": id,
        "value": generate_random_string()
    }
    response = requests.post(url, headers=headers, data=json.dumps(data))
    print(f"POST ID: {id}, Status Code: {response.status_code}, Response: {response.text}")

# Function to send a GET request
def send_get_request(id):
    url = f"http://localhost:18080/get/{id}"
    headers = {
        "Accept": "application/json"
    }
    response = requests.get(url, headers=headers)
    print(f"GET ID: {id}, Status Code: {response.status_code}, Response: {response.text}")

# Main function to run the requests in 100 threads
def main():
    with ThreadPoolExecutor(max_workers=100) as executor:
        # Sending POST requests
        executor.map(send_post_request, range(1, 1001))
        # Sending GET requests
        executor.map(send_get_request, range(1, 1001))

if __name__ == "__main__":
    main()