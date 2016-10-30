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
            <h1>Add Actor/Movie Relation</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label for="directorID">Select Director</label>
                    <select class="form-control" name="directorID">
                        <option value="Default">Please select</option>
                        <?php
                            // connecting to db
                            $db = new mysqli('localhost', 'cs143', '', 'CS143');
                            if ($db->connect_errno > 0) {
                                die('Unable to connect to database [' . $db->connect_error . ']');
                            }

                            $query = "SELECT id, first, last FROM Director;";
                            $result = $db->query($query);
                            if (!$result) {
                                $errmsg = $db->error;
                                print "Query failed: $errmsg<br />";
                                exit(1);
                            }

                            $row = $result->fetch_assoc();
                            // populates drop down selection
                            while (true) {
                                echo '<option value="'.$row['id'].'">'.$row['first']." ".$row['last']."</option>";
                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                        ?>
                    </select>
                </div>
                <div class="form-group">
                    <label for="movieID">Select Movie</label>
                    <select class="form-control" name="movieID">
                        <option value="Default">Please select</option>
                        <?php
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
                                echo '<option value="'.$row['id'].'">'.$row['title']."</option>";
                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                        ?>
                    </select>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>
            <br>

            <?php
                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    // check the button is clicked
                    if (isset($_GET['submit'])) {
                        $directorID = $_REQUEST['directorID'];
                        $movieID = $_REQUEST['movieID'];

                        // input checks
                        if ($directorID == "Default") {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select an Director!</div>';
                            exit(1);
                        }

                        if ($movieID == "Default") {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select a Movie!</div>';
                            exit(1);
                        }

                        // connecting to db
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }

                        // query
                        $query = "INSERT INTO MovieDirector VALUES (".$movieID.", ".$directorID.");";
                        
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