<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1">
    
        <title>CS143 Project 1C</title>

        <!-- CSS -->
        <link href="css/bootstrap.min.css" rel="stylesheet">
        <link href="css/custom.css" rel="stylesheet">
    </head>

    <body>
        <?php
            // header navigation bar
            include_once('header.php');
        ?>

        <div class="container main">
            <h1>Add Movie</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label for="tile">Movie Title</label>
                    <input type="text" class="form-control" placeholder="" name="title">
                </div>
                <div class="form-group">
                    <label for="company">Production Company</label>
                    <input type="text" class="form-control" placeholder="" name="company">
                </div>
                <div class="form-group">
                    <label for="year">Release Year</label>
                    <input type="text" class="form-control" placeholder="yyyy" name="year">
                </div>
                <div class="form-group">
                    <label for="rating">MPAA Rating</label>
                    <select class="form-control" name="rating">
                        <option value="Default">Please select</option>
                        <option value="G">G</option>
                        <option value="NC-17">NC-17</option>
                        <option value="PG">PG</option>
                        <option value="PG-13">PG-13</option>
                        <option value="R">R</option>
                        <option value="surrendere">surrendere</option>
                    </select>
                </div>
                <div class="form-group">
                    <label>Movie Genre (select one or more):</label>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Action">Action</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Adult">Adult</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Adventure">Adventure</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Animation">Animation</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Comedy">Comedy</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Crime">Crime</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Documentary">Documentary</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Drama">Drama</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Family">Family</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Fantasy">Fantasy</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Horror">Horror</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Musical">Musical</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Mystery">Mystery</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Romance">Romance</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Short">Short</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Thriller">Thriller</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="War">War</label></div>
                    <div class="checkbox"><label><input type="checkbox" name="genre[]" value="Western">Western</label></div>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>
            <br>

            <?php
                define("SQL_YEAR_REGEX", "/\d{4}/");

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    // check the button is clicked
                    if (isset($_GET['submit'])) {
                        $title = $_REQUEST['title'];
                        $company = $_REQUEST['company'];
                        $year = $_REQUEST['year'];
                        $rating = $_REQUEST['rating'];
                        $genre = $_REQUEST['genre'];

                        // empty checks
                        if (empty($title)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Title can not be empty!</div>';
                            exit(1);
                        }

                        if (empty($company)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Production company can not be empty!</div>';
                            exit(1);
                        }

                        if (empty($year)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Year can not be empty!</div>';
                            exit(1);
                        } else if (!preg_match(SQL_YEAR_REGEX, $year)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Wrong year format!</div>';
                            exit(1);
                        }

                        if ($rating == "Default") {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select movie rating!</div>';
                            exit(1);
                        }

                        if (empty($genre)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select at least one genre!</div>';
                            exit(1);
                        }


                        // connecting to db
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }

                        // retrieve id
                        $maxMovieIDQuery = $db->query("SELECT id FROM MaxMovieID");
                        if (!$maxMovieIDQuery) {
                            $errmsg = $db->error;
                            print "Unable to retrieve MaxMovieID";
                            print "Query failed: $errmsg<br />";
                            exit(1);
                        }
                        $maxMovieID = $maxMovieIDQuery->fetch_assoc()[id];
                        $newMovieID = $maxMovieID+1;
                        
                        // handle special char in name
                        $title = str_replace("'", "\'", $title);

                        // query
                        $query = "INSERT INTO Movie VALUES (".$newMovieID.", '".$title."', ".$year.", '".$rating."', '".$company."');";
                        $queryID = "UPDATE MaxMovieID SET id = id+1";


                        // executing query
                        $result = $db->query($query);
                        if (!result) {
                            $errmsg = $db->error;
                            echo '<div class="alert alert-danger"><strong>Error!</strong><p>Query failed: '.$errmsg.'</p><p>Query: $query</p></div>';
                        } else {
                            // update MaxMovieID
                            // TODO: suppose this pass
                            $db->query($queryID);

                            echo '<div class="alert alert-success"><p><strong>Success!</strong></p><p>Query: '.$query.'</p>';

                            // insert genre
                            foreach ($genre as $ele) {
                                $genreQuery = "INSERT INTO MovieGenre VALUES (".$newMovieID.", '".$ele."');";
                                // TODO: suppose this pass
                                $db->query($genreQuery);
                                echo '<p>Query: '.$genreQuery.'</p>';
                            }

                            echo '</div>';     
                        }
                    }
                }
            ?>

        </div>

        <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
        <!-- Include all compiled plugins (below), or include individual files as needed -->
        <script src="js/bootstrap.min.js"></script>
    </body>
</html>