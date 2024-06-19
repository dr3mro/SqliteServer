import http from 'k6/http';
import { check } from 'k6';

export let options = {
    vus: 1000,
    duration: '30s',
};

export default function () {
    // Generate a random integer between 1 and 1000
    let randomNumber = Math.floor(Math.random() * 1000) + 1;
    
    let url = `http://localhost:8080/get/${randomNumber}`;
    
    let res = http.get(url);
    
    check(res, {
        'status is 200': (r) => r.status === 200,
    });
}
