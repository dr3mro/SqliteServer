import http from 'k6/http';
import { check } from 'k6';

export let options = {
    vus: 1000,
    duration: '120s',
};

export default function () {
    for (let i = 1; i <= 1000; i++) {
        let url = `http://localhost:8080/get/${i}`;
        
        let res = http.get(url);
        
        check(res, {
            'status is 200': (r) => r.status === 200,
        });
    }
}

