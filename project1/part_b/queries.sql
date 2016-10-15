-- The names of all actors in the movie Die Another Day
SELECT      CONCAT(first, ' ', last)
FROM        Actor
INNER JOIN  MovieActor
ON          Actor.id=MovieActor.aid
INNER JOIN  Movie
ON          MovieActor.mid=Movie.id
WHERE       Movie.title='Die Another Day';

-- The count of actors who have acted in multiple movies
SELECT  COUNT(*)
FROM    (   
            SELECT      *
            FROM        MovieActor
            GROUP BY    aid
            HAVING      COUNT(mid) > 1
        ) AS Actors;

-- The names of actors who have acted in comedy, romance,
-- and action films
SELECT      CONCAT(first, ' ', last)
FROM        Actor a
INNER JOIN  MovieActor ma
ON          a.id=ma.aid
INNER JOIN  MovieGenre mg
ON          ma.mid=mg.mid
WHERE       mg.genre IN ('Comedy', 'Romance', 'Action')
GROUP BY    ma.aid
HAVING      COUNT(distinct mg.genre) = 3;

-- SELECT      DISTINCT CONCAT(first, ' ', last)
-- FROM        (
--                 SELECT  aid
--                 FROM    MovieGenre mg, MovieActor ma
--                 WHERE   mg.mid=ma.mid AND mg.genre='Comedy'
--             ) c
-- INNER JOIN  (
--                 SELECT  aid
--                 FROM    MovieGenre mg, MovieActor ma
--                 WHERE   mg.mid=ma.mid AND mg.genre='Romance'
--             ) r
-- ON          c.aid=r.aid
-- INNER JOIN  (
--                 SELECT  aid
--                 FROM    MovieGenre mg, MovieActor ma
--                 WHERE   mg.mid=ma.mid AND mg.genre='Action'
--             ) a
-- ON          c.aid=a.aid
-- INNER JOIN  Actor
-- ON          c.aid=Actor.id
