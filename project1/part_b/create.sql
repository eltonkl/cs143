CREATE TABLE Movie(
    id INT,
    title VARCHAR(100),
    year INT,
    rating VARCHAR(10),
    company VARCHAR(50)
) ENGINE = INNODB;

CREATE TABLE Actor(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    sex VARCHAR(6),
    dob DATE,
    dod DATE
) ENGINE = INNODB;

CREATE TABLE Director(
    id INT,
    last VARCHAR(20),
    first VARCHAR(20),
    dob DATE,
    dod DATE
) ENGINE = INNODB;

CREATE TABLE MovieGenre(
    mid INT,
    genre VARCHAR(20)
) ENGINE = INNODB;

CREATE TABLE MovieDirector(
    mid INT,
    did INT
) ENGINE = INNODB;

CREATE TABLE MovieActor(
    mid INT,
    aid INT,
    role VARCHAR(50)
) ENGINE = INNODB;

CREATE TABLE Review(
    name VARCHAR(20),
    time TIMESTAMP,
    mid INT,
    rating INT,
    comment VARCHAR(500)
) ENGINE = INNODB;

CREATE TABLE MaxPersonID(
    id INT
) ENGINE = INNODB;

CREATE TABLE MaxMovieID(
    id INT
) ENGINE = INNODB;
