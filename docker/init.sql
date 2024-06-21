-- init.sql
-- Create the sequence starting from 100000
CREATE SEQUENCE patient_id_seq START 100000;

-- Create the table with correct usage of sequence for id column
CREATE TABLE IF NOT EXISTS personal_history (
    id INTEGER PRIMARY KEY DEFAULT nextval('patient_id_seq'),
    name VARCHAR(255) UNIQUE,
    phone VARCHAR(255) UNIQUE,
    json JSONB,
    created_on TIMESTAMP WITH TIME ZONE DEFAULT now(),
    modified_on TIMESTAMP WITH TIME ZONE
);