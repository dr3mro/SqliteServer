-- init.sql
CREATE TABLE IF NOT EXISTS personal_history (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) UNIQUE,
    phone VARCHAR(255) UNIQUE,
    json JSONB,
    created_on TIMESTAMP WITH TIME ZONE default now(),
    modified_on TIMESTAMP WITH TIME ZONE
);

