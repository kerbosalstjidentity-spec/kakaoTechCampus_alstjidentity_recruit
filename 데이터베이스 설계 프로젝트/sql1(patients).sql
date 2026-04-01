DELIMITER $$
create procedure insert_patient(in p_name varchar(255),in p_date_of_birth DATE, 
in p_gender varchar(255))
begin 
  INSERT INTO patients (name, date_of_birth_, gender) values (p_name, p_date_of_birth, p_gender);
END 
$$
DELiMiTER ;

DELIMITER $$
create procedure delete_patient (in p_id INT)
begin
delete from patients
where id=p_id;
end
$$DELiMiTER ;

DELIMiTER // 
create procedure update_patient(
 In p_id INT, IN p_name varchar(255), In p_date_of_birth DATE, In p_gender varchar(255)
)
BEgin
update patients set name=p_name, date_of_birth=p_date_of_birth where  id=p_id;
end
$$ DELIMiTER ;

-- call insert_patient ('Alice smith','1992-03-30','female');
-- call delete_patient (1);
-- call update_patient(2,'Rober johnson','1992-03-30','male');



delimiter $$

CREATE PROCEDURE insert_patient_with_details(
    IN p_name VARCHAR(255),
    IN p_date_of_birth DATE,
    IN p_gender VARCHAR(255),
    IN p_resident_registeration_number VARCHAR(255),
    IN p_phone_number VARCHAR(255)
)
BEGIN
    DECLARE p_id INT;
    DECLARE v_valid BOOLEAN DEFAULT FALSE;

    -- 주민등록번호 유효성 검사 로직 (간단한 예)
    IF LENGTH(p_resident_registeration_number) = 14 AND p_resident_registeration_number REGEXP '^[0-9]{6}-[0-9]{7}$' THEN
	     IF LENGTH(p_phone_number) = 13 AND p_phone_number REGEXP '^010-[0-9]{4}-[0-9]{4}$' THEN
              SET v_valid = TRUE;
		 end if;
    END IF;

    IF v_valid = FALSE THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid registration number format';
    ELSE
        -- 환자 존재 여부 검사
        IF EXISTS (SELECT patient_id FROM patient_identification WHERE resident_registration_number = p_resident_registeration_number) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Resident registration number already exists';
        ELSE
            -- patients 테이블에 데이터 삽입
            INSERT INTO patients (name, date_of_birth, gender) VALUES (p_name, p_date_of_birth, p_gender);
            SET p_id = LAST_INSERT_ID();

            -- patient_identification 테이블에 데이터 삽입
            INSERT INTO patient_identification (patient_id, resident_registration_number) VALUES (p_id, p_resident_registeration_number);

            -- patient_phones 테이블에 데이터 삽입
            INSERT INTO patient_phones (patient_id, phone_number) VALUES (p_id, p_phone_number);

            -- patients_Anonymous 테이블에 데이터 삽입
            INSERT INTO patients_Anonymous (id, patient_id) VALUES (p_id, p_id);
        END IF;
    END IF;
END
$$
delimiter ;

CALL insert_patient_with_details('John Doe', '1985-05-15', 'female', '111111-4567890','010-1234-567');
CALL insert_patient_with_details('John Doe', '1985-05-15', 'female', '111111-456789','010-1234-567');
CALL insert_patient_with_details('John Doe', '1985-05-15', 'female', '111111-4567890','010-1234-5678');








