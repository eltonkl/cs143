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
            <h1>Add Actor or Director</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label>Occupation:</label>
                    <label class="radio-inline">
                        <input type="radio" name="type" value="Actor">Actor
                    </label>
                    <label class="radio-inline">
                        <input type="radio" name="type" value="Director"> Director
                    </label>
                </div>
                <div class="form-group">
                    <label for="firstName">First Name</label>
                    <input type="text" class="form-control" placeholder="" name="firstName">
                </div>
                <div class="form-group">
                    <label for="lastName">Last Name</label>
                    <input type="text" class="form-control" placeholder="" name="lastName">
                </div>
                <div class="form-group">
                    <label for="lastName">Date of Birth</label>
                    <input type="text" class="form-control" placeholder="yyyy-mm-dd" name="dob">
                    <p class="help-block"></p>
                </div>
                <div class="form-group">
                    <label for="lastName">Date of Death</label>
                    <input type="text" class="form-control" placeholder="yyyy-mm-dd" name="dod">
                    <p class="help-block">Or leave blank if still alive</p>
                </div>
                <div class="form-group">
                    <label>Gender:</label>
                    <label class="radio-inline">
                        <input type="radio" name="gender" value="Male">Male
                    </label>
                    <label class="radio-inline">
                        <input type="radio" name="gender" value="Female">Female
                    </label>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>
            <br>

            <?php
                define("SQL_DATE_REGEX", "/\d{4}-\d{2}-\d{2}/");

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    // check the button is clicked
                    if (isset($_GET['submit'])) {
                        $type = $_REQUEST['type'];
                        $firstName = $_REQUEST['firstName'];
                        $lastName = $_REQUEST['lastName'];
                        $dob = $_REQUEST['dob'];
                        $dod = $_REQUEST['dod'];
                        $gender = $_REQUEST['gender'];

                        // empty checks
                        if (empty($type)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Please select occupation.</div>';
                            exit(1);
                        }

                        if (empty($firstName)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> First Name is empty!</div>';
                            exit(1);
                        }

                        if (empty($lastName)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Last Name is empty!</div>';
                            exit(1);
                        }

                        if ($type == "Actor" && empty($gender)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Actor should have a gender.</div>';
                            exit(1);
                        }

                        if (empty($dob)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Date of Birth is empty!</div>';
                            exit(1);
                        } else if (!preg_match(SQL_DATE_REGEX, $dob)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Wrong date of birth format!</div>';
                            exit(1);
                        }

                        if (empty($dod)) {
                            $dod = "NULL";
                        } else if (!preg_match(SQL_DATE_REGEX, $dod)) {
                            echo '<div class="alert alert-danger" role="alert"><strong>Error!</strong> Wrong date of death format!</div>';
                            exit(1);
                        }
                        

                        // connecting to db
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }

                        // retrieve id
                        $maxPersonIDQuery = $db->query("SELECT id FROM MaxPersonID");
                        if (!$maxPersonIDQuery) {
                            $errmsg = $db->error;
                            print "Unable to retrieve MaxPersonID";
                            print "Query failed: $errmsg<br />";
                            exit(1);
                        }
                        $maxPersonID = $maxPersonIDQuery->fetch_assoc()[id];
                        
                        // handle special char in name
                        $firstName = str_replace("'", "\'", $firstName);
                        $lastName = str_replace("'", "\'", $lastName);

                        // query
                        if ($type == "Actor") {
                            $query = "INSERT INTO ".$type." VALUES (".($maxPersonID+1).", '".$lastName."', '".$firstName."', '".$gender."', '".$dob."'";
                        } else {
                            // Director (no gender)
                            $query = "INSERT INTO ".$type." VALUES (".($maxPersonID+1).", '".$lastName."', '".$firstName."', '".$dob."'";
                        }

                        // dod NULL handle
                        if ($dod == "NULL") {
                            $query = $query.", ".$dod.");";
                        } else {
                            $query = $query.", '".$dod."');";
                        }

                        // MaxPersonID query
                        $queryID = "UPDATE MaxPersonID SET id = id+1";

                        // executing query
                        $result = $db->query($query);
                        if (!result) {
                            $errmsg = $db->error;
                            echo '<div class="alert alert-danger"><strong>Error!</strong><p>Query failed: '.$errmsg.'</p><p>Query: $query</p></div>';
                        } else {
                            $db->query($queryID);
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
