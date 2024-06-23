-- init.sql
-- Create the sequence starting from 100000
CREATE SEQUENCE patient_id_seq START 100000;

-- Create the table with correct usage of sequence for id column
CREATE TABLE IF NOT EXISTS patients_basic_data (
    id INTEGER PRIMARY KEY DEFAULT nextval('patient_id_seq'),
    basic_data JSONB,
    created_on timestamptz DEFAULT current_timestamp,
    updated_on timestamptz DEFAULT current_timestamp
);

-- Optional: Create a trigger to update `updated_at` on every update
CREATE OR REPLACE FUNCTION update_updated_on_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_on = current_timestamp;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER patients_basic_data_table_updated_on
BEFORE UPDATE ON patients_basic_data
FOR EACH ROW
EXECUTE FUNCTION update_updated_on_column();