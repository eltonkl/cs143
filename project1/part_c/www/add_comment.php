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
                    <select class="form-control" name="id">
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
                            // populates drop down selection
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
                    <label for="name">Your Name</label>
                    <input type="text" class="form-control" placeholder="" name="name">
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
                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    // check the button is clicked
                    if (isset($_GET['submit'])) {
                        $id = $_REQUEST['id'];
                        $name = $_REQUEST['name'];
                        $rating = $_REQUEST['rating'];
                        $comment = $_REQUEST['comment'];

                        // input checks
                        if ($id == "Default") {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select a movie!</div>';
                            exit(1);
                        }

                        if (empty($name)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please enter your name!</div>';
                            exit(1);
                        }

                        if ($rating == "Default") {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please give your rating on the movie!</div>';
                            exit(1);
                        }

                        if (empty($comment)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please leave a comment!</div>';
                            exit(1);
                        } else if (strlen($comment) > 500) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Comment should be less than 500 characters!</div>';
                            exit(1);
                        }


                        // connecting to db
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }
                        
                        // handle special char
                        $name = str_replace("'", "\'", $name);
                        $comment = str_replace("'", "\'", $comment);

                        // query
                        $query = "INSERT INTO Review VALUES ('".$name."', ".time().", ".$id.", ".$rating.", '".$comment."');";

                        // executing query
                        $result = $db->query($query);
                        if (!result) {
                            $errmsg = $db->error;
                            echo '<div class="alert alert-danger"><strong>Error!</strong><p>Query failed: '.$errmsg.'</p><p>Query: $query</p></div>';
                        } else {
                            echo '<div class="alert alert-success"><p><strong>Success!</strong></p><p>Query: '.$query.'</p></div>';
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