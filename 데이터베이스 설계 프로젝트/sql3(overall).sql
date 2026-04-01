DELIMITER //

CREATE PROCEDURE AddDoctor (
    IN doc_id INT,
    IN doc_name VARCHAR(255),
    IN spec_id INT,
    IN acquisition DATE
)
BEGIN
    DECLARE dept_id INT;

    -- Speciality가 존재하는지 확인
    IF NOT EXISTS (SELECT 1 FROM specialities WHERE id = spec_id) THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Invalid speciality ID';
    END IF;

    -- Speciality와 연관된 department ID 가져오기
    SELECT depart_id INTO dept_id FROM specialities WHERE id = spec_id;

    -- Department가 존재하는지 확인
    IF NOT EXISTS (SELECT 1 FROM departments WHERE id = dept_id) THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Invalid department ID';
    END IF;

    -- 모든 조건을 만족하면 doctor 삽입
    INSERT INTO doctors (id, name, speciality_id, Acquistion_date) 
    VALUES (doc_id, doc_name, spec_id, acquisition);
END //

DELIMITER ;

INSERT INTO departments (id, name) VALUES (1, 'Cardiology');
INSERT INTO departments (id, name) VALUES (2, 'Neurology');

INSERT INTO specialities (id, term, depart_id) VALUES (1, 'Cardiac Surgery', 1);
INSERT INTO specialities (id, term, depart_id) VALUES (2, 'Neurosurgery', 2);

-- 저장 프로시저 호출 - 올바른 입력
CALL AddDoctor(1, 'Dr. John Doe', 1, '2024-06-01');

-- 저장 프로시저 호출 - 잘못된 speciality ID
CALL AddDoctor(2, 'Dr. Jane Smith', 99, '2024-06-01');

-- 저장 프로시저 호출 - 잘못된 department ID
-- 이 경우는 specialities 테이블에 존재하는 dept_id가 departments에 존재하지 않는 경우를 의미
INSERT INTO specialities (id, term, depart_id) VALUES (3, 'Unknown Speciality', 99);
CALL AddDoctor(3, 'Dr. Alice Brown', 3, '2024-06-01');

DELIMITER //

CREATE PROCEDURE AddSpeciality (
    IN spec_id INT,
    IN spec_term VARCHAR(255),
    IN dept_id INT
)
BEGIN
    -- Department가 존재하는지 확인
    IF NOT EXISTS (SELECT 1 FROM departments WHERE id = dept_id) THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Invalid department ID';
    END IF;

    -- 모든 조건을 만족하면 speciality 삽입
    INSERT INTO specialities (id, term, depart_id) 
    VALUES (spec_id, spec_term, dept_id);
END //

DELIMITER ;


-- 예제 데이터 삽입
INSERT INTO departments (id, name, suffixNumber) VALUES (1, 'Cardiology', 101);
INSERT INTO departments (id, name, suffixNumber) VALUES (2, 'Neurology', 102);

-- 저장 프로시저 호출 - 올바른 입력
CALL AddSpeciality(1, 'Cardiac Surgery', 1);
CALL AddSpeciality(2, 'Neurosurgery', 2);

-- 저장 프로시저 호출 - 잘못된 department ID
CALL AddSpeciality(3, 'Unknown Speciality', 99);


CREATE PROCEDURE AddMedication (
    IN med_id INT,
    IN med_name VARCHAR(255),
    IN med_dosage INT,
    IN med_duration DATE,
    IN med_suffixNumber INT,
    IN med_perIntake INT
)
BEGIN
    INSERT INTO medications (id, medication_name, dosage, duration, suffixNumber, perIntake) 
    VALUES (med_id, med_name, med_dosage, med_duration, med_suffixNumber, med_perIntake);
END //
DELIMITER ;

DELIMITER //

CREATE PROCEDURE AddPrescription (
    IN pres_id INT,
    IN pres_date DATETIME(3),
    IN pres_comments VARCHAR(255),
    IN med_id INT
)
BEGIN
    -- Medication이 존재하는지 확인
    IF NOT EXISTS (SELECT 1 FROM medications WHERE id = med_id) THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Invalid medication ID';
    END IF;

    INSERT INTO prescriptions (id, prescription_date, comments, medication_id) 
    VALUES (pres_id, pres_date, pres_comments, med_id);
END //


CREATE PROCEDURE AddPatientGuardian (
    IN p_patient_id INT,
    IN p_guardian_id INT,
    IN p_relationship VARCHAR(255)
)
BEGIN
    -- 유효한 관계인지 확인
    IF p_relationship NOT IN ('부모', '배우자', '형제', '후견인') THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'Invalid relationship type';
    END IF;

    -- 삽입
    INSERT INTO patient_guardians (patient_id, guardian_id, relationship) 
    VALUES (p_patient_id, p_guardian_id, p_relationship);
END //

DELIMITER ;
