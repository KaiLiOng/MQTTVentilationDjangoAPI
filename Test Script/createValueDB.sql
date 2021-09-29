CREATE TABLE "api_value" (
	"id"	integer NOT NULL,
	"value_date"	datetime NOT NULL,
	"value"	real NOT NULL,
	"sensor_id"	varchar(200) NOT NULL,
	"patient_id"	integer NOT NULL,
	FOREIGN KEY("sensor_id") REFERENCES "api_sensor"("sensor_name") DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY("patient_id") REFERENCES "api_patient"("id") DEFERRABLE INITIALLY DEFERRED,
	PRIMARY KEY("id" AUTOINCREMENT)
);