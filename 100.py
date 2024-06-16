import requests
import json
import random
import string

# Define the URL and headers
url = "http://localhost:18080/post"
headers = {
    "Accept": "application/json",
    "Content-type": "application/json"
}

# Function to generate a random string
def generate_random_string(length=8):
    letters = string.ascii_letters
    return ''.join(random.choice(letters) for _ in range(length))

# Function to perform a POST request
def post_request(id, value):
    data = {
        "id": id,
        "value": value
    }
    response = requests.post(url, headers=headers, data=json.dumps(data))
    return response.status_code, response.json() if response.status_code == 200 else response.text

# Loop to create 100 POST requests
for i in range(1, 101):
    random_value = generate_random_string()
    status, response = post_request(i, random_value)
    print(f"POST ID: {i}, Status: {status}, Response: {response}")
