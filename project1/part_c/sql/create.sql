CREATE TABLE Movie(
    id INT,
    title VARCHAR(100),
    year INT,
    rating VARCHAR(10),
    company VARCHAR(50),
    PRIMARY KEY(id)     -- Movie id should be a primary key as it uniquely identifies each Movie
) ENGINE = INNODB;

CREATE TABLE Actor(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    sex VARCHAR(6),
    dob DATE,
    dod DATE,
    PRIMARY KEY(id),    -- Actor id should be a primary key as it uniquely identifies each Actor
    CHECK (dob <= CURRENT_DATE())   -- date of birth should not be in the future
) ENGINE = INNODB;

CREATE TABLE Director(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    dob DATE,
    dod DATE,
    PRIMARY KEY(id),    -- Director id should be a primary key as it uniquely identifies each Director
    CHECK (dob <= dod OR dob IS NULL OR dod IS NULL)    -- If dod and dob is NOT NULL, dob should not be after dod
) ENGINE = INNODB;

CREATE TABLE MovieGenre(
    mid INT,
    genre VARCHAR(20),
    FOREIGN KEY(mid) REFERENCES Movie(id)   -- mid should be a foreign key referencing Movie id
) ENGINE = INNODB;

CREATE TABLE MovieDirector(
    mid INT,
    did INT,
    FOREIGN KEY(mid) REFERENCES Movie(id),  -- mid should be a foreign key referencing Movie id
    FOREIGN KEY(did) REFERENCES Director(id)-- did should be a foreign key referencing Director id
) ENGINE = INNODB;

CREATE TABLE MovieActor(
    mid INT,
    aid INT,
    role VARCHAR(50),
    FOREIGN KEY(mid) REFERENCES Movie(id),  -- mid should be a foreign key referencing Movie id
    FOREIGN KEY(aid) REFERENCES Actor(id)   -- aid should be a foreign key referencing Director id
) ENGINE = INNODB;

CREATE TABLE Review(
    name VARCHAR(20),
    time TIMESTAMP,
    mid INT,
    rating INT,
    comment VARCHAR(500),
    FOREIGN KEY(mid) REFERENCES Movie(id),  -- mid should be a foreign key referencing Movie id
    CHECK (0 <= rating AND rating <= 5)     -- rating should be between 0 and 5, inclusive
) ENGINE = INNODB;

CREATE TABLE MaxPersonID(
    id INT
) ENGINE = INNODB;

CREATE TABLE MaxMovieID(
    id INT
) ENGINE = INNODB;
