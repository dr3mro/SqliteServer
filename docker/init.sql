-- init.sql
CREATE TABLE IF NOT EXISTS personal_history (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255),
    phone VARCHAR(255)[],
    json JSONB
);