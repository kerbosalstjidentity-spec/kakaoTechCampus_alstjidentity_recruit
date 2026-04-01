DELIMITER //

CREATE PROCEDURE insert_guardian_with_details(
    IN p_id INT,
    IN g_name VARCHAR(255),
    IN g_phone_number VARCHAR(255),
    IN relation varchar(255)
)
BEGIN
    DECLARE g_id INT;
    DECLARE v_valid BOOLEAN DEFAULT FALSE;

    -- Phone number validation logic
    IF LENGTH(g_phone_number) = 13 AND g_phone_number REGEXP '^010-[0-9]{4}-[0-9]{4}$' THEN
        SET v_valid = TRUE;
    END IF;

    IF v_valid = FALSE THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid phone number format';
    ELSE
        START TRANSACTION;
        BEGIN
            -- Insert into guardians table
            INSERT INTO guardians (name) VALUES (g_name);
            SET g_id = LAST_INSERT_ID();
            
            -- Insert into guardian_phones table
            INSERT INTO guardian_phones (guardian_id, phone_number) VALUES (g_id, g_phone_number);

            -- Check if patient exists and if patient_guardians already has an entry
            IF NOT EXISTS (SELECT * FROM patients WHERE id = p_id) THEN
                ROLLBACK;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Patient does not exist';
            ELSEIF EXISTS (SELECT * FROM patient_guardians WHERE patient_id = p_id AND guardian_id = g_id) THEN
                ROLLBACK;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'This guardian is already associated with the patient';
            ELSE
                -- If everything is valid, commit the transaction
                 insert into patient_guardians values(p_id, g_id, relation);
                COMMIT;
            END IF;
        END;
    END IF;
END //

DELIMITER ;
-- drop procedure insert_guardian_with_details;

call insert_guardian_with_details('drew' ,'010-1111-132');



