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
            <h1>Add Review</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label for="id">Movie Title</label>
                    <select class="form-control" name="rating">
                        <option value="Default">Please select</option>
                        <?php
                            // get movie ID
                            if (isset($_GET['movieID'])) {
                                $movieID = $_GET['movieID'];
                            } else {
                                $movieID = 0;
                            }

                            // connecting to db
                            $db = new mysqli('localhost', 'cs143', '', 'CS143');
                            if ($db->connect_errno > 0) {
                                die('Unable to connect to database [' . $db->connect_error . ']');
                            }

                            $query = "SELECT id, title FROM Movie;";
                            $result = $db->query($query);
                            if (!$result) {
                                $errmsg = $db->error;
                                print "Query failed: $errmsg<br />";
                                exit(1);
                            }

                            $row = $result->fetch_assoc();
                            
                            while (true) {
                                if ($row['id'] == $movieID) {
                                    // set to selected
                                    echo '<option value="'.$row['id'].'" selected="selected">'.$row['title']."</option>";

                                } else {
                                    echo '<option value="'.$row['id'].'">'.$row['title']."</option>";
                                }

                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                        ?>
                    </select>
                </div>
                <div class="form-group">
                    <label for="tile">Your Name</label>
                    <input type="text" class="form-control" placeholder="" name="title">
                </div>
                <div class="form-group">
                    <label for="rating">Rating</label>
                    <select class="form-control" name="rating">
                        <option value="Default">Please select</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                        <option value="3">3</option>
                        <option value="4">4</option>
                        <option value="5">5</option>
                    </select>
                </div>
                <div class="form-froup">
                  <textarea class="form-control" name="comment" rows="5" placeholder="Please limit your review to 500 characters"></textarea>
                  <br> 
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
                        // if (empty($title)) {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Title can not be empty!</div>';
                        //     exit(1);
                        // }

                        // if (empty($company)) {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Production company can not be empty!</div>';
                        //     exit(1);
                        // }

                        // if (empty($year)) {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Year can not be empty!</div>';
                        //     exit(1);
                        // } else if (!preg_match(SQL_YEAR_REGEX, $year)) {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Wrong year format!</div>';
                        //     exit(1);
                        // }

                        // if ($rating == "Default") {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select movie rating!</div>';
                        //     exit(1);
                        // }

                        // if (empty($genre)) {
                        //     echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select at least one genre!</div>';
                        //     exit(1);
                        // }


                        // // connecting to db
                        // $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        // if ($db->connect_errno > 0) {
                        //     die('Unable to connect to database [' . $db->connect_error . ']');
                        // }

                        // // retrieve id
                        // $maxMovieIDQuery = $db->query("SELECT id FROM MaxMovieID");
                        // if (!$maxMovieIDQuery) {
                        //     $errmsg = $db->error;
                        //     print "Unable to retrieve MaxMovieID";
                        //     print "Query failed: $errmsg<br />";
                        //     exit(1);
                        // }
                        // $maxMovieID = $maxMovieIDQuery->fetch_assoc()[id];
                        // $newMovieID = $maxMovieID+1;
                        
                        // // handle special char in name
                        // $title = str_replace("'", "\'", $title);

                        // // query
                        // $query = "INSERT INTO Movie VALUES (".$newMovieID.", '".$title."', ".$year.", '".$rating."', '".$company."');";
                        // $queryID = "UPDATE MaxMovieID SET id = id+1";


                        // // executing query
                        // $result = $db->query($query);
                        // if (!result) {
                        //     $errmsg = $db->error;
                        //     echo '<div class="alert alert-danger"><strong>Error!</strong><p>Query failed: '.$errmsg.'</p><p>Query: $query</p></div>';
                        // } else {
                        //     // update MaxMovieID
                        //     // TODO: suppose this pass
                        //     $db->query($queryID);

                        //     echo '<div class="alert alert-success"><p><strong>Success!</strong></p><p>Query: '.$query.'</p>';

                        //     // insert genre
                        //     foreach ($genre as $ele) {
                        //         $genreQuery = "INSERT INTO MovieGenre VALUES (".$newMovieID.", '".$ele."');";
                        //         // TODO: suppose this pass
                        //         $db->query($genreQuery);
                        //         echo '<p>Query: '.$genreQuery.'</p>';
                        //     }

                        //     echo '</div>';     
                        // }
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