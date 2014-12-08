BEGIN; 

DROP INDEX IF EXISTS idx_measurements_id;
DROP INDEX IF EXISTS idx_samples_id;
DROP INDEX IF EXISTS idx_samples_measurement_id;
DROP INDEX IF EXISTS idx_sample_groups_id;
DROP INDEX IF EXISTS idx_sample_groups_measurement_id;
DROP TABLE IF EXISTS samples;
DROP TABLE IF EXISTS sample_groups;
DROP TABLE IF EXISTS measurements;
DROP SEQUENCE IF EXISTS seq_measurements_id;
DROP SEQUENCE IF EXISTS seq_samples_id;
DROP SEQUENCE IF EXISTS seq_sample_groups_id;

CREATE SEQUENCE seq_measurements_id;
CREATE TABLE measurements (
id INTEGER PRIMARY KEY DEFAULT nextval('seq_measurements_id'),
  name TEXT NOT NULL
);

CREATE SEQUENCE seq_samples_id;
CREATE TABLE samples (
  id INTEGER PRIMARY KEY DEFAULT nextval('seq_samples_id'),
  measurement_id INTEGER NOT NULL,
  value FLOAT NOT NULL
);

CREATE SEQUENCE seq_sample_groups_id;
CREATE TABLE sample_groups (
  id INTEGER PRIMARY KEY DEFAULT nextval('seq_sample_groups_id'),
  measurement_id INTEGER NOT NULL,
  values FLOAT[] NOT NULL
);

INSERT INTO measurements
(name)
SELECT  a::text
FROM    generate_series(1, 100) AS s(a)
;

INSERT INTO samples
(measurement_id, value)
SELECT  m.id, random() * 20000000000 - 10000000000
FROM    measurements m
CROSS JOIN generate_series(1, 1000000)
;

INSERT INTO sample_groups
(measurement_id, values)
SELECT  m.id, (SELECT array_agg(random() * 20000000000 - 10000000000) FROM generate_series(1, 1000000))
FROM    measurements m
;

CREATE INDEX idx_measurements_id ON measurements (id);
CREATE INDEX idx_samples_id ON samples (id);
CREATE INDEX idx_samples_measurement_id ON samples (measurement_id);
CREATE INDEX idx_sample_groups_id ON sample_groups (id);
CREATE INDEX idx_sample_groups_measurement_id ON sample_groups (measurement_id);

COMMIT;

