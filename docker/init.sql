-- init.sql
-- Create the sequence starting from 100000
CREATE SEQUENCE patient_id_seq START 100000;

-- Set starting user_id from 1000
CREATE SEQUENCE user_id_seq START 1000;


-- Create the table with correct usage of sequence for id column
CREATE TABLE IF NOT EXISTS patients (
    id INTEGER PRIMARY KEY DEFAULT nextval('patient_id_seq'),
    basic_data JSONB,
    health_data JSONB,
    appointments_data JSONB,
    created_on timestamptz DEFAULT current_timestamp,
    updated_on timestamptz DEFAULT current_timestamp
);

-- Create the table with correct usage of sequence for id column
CREATE TABLE users (
    user_id INTEGER PRIMARY KEY DEFAULT nextval('user_id_seq'),
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL,
    user_data JSONB
);

-- Optional: Create a trigger to update `updated_at` on every update
CREATE OR REPLACE FUNCTION update_updated_on_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_on = current_timestamp;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER patients_table_updated_on
BEFORE UPDATE ON patients
FOR EACH ROW
EXECUTE FUNCTION update_updated_on_column();