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
            <?php
                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    $actorID = $_REQUEST['actorID'];

                    if (!empty($actorID)) {
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }

                        $query = "SELECT * FROM Actor WHERE id=" . $actorID;
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }

                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h1>No results!</h1>";
                            exit(1);
                        }
                        else
                            echo "<h1>" . $row["first"] . " " . $row["last"] . "</h1>";

                        echo "<br>";

                        $coldbnames = ["sex", "dob", "dod"];
                        $colheaders = ["Gender", "Date of Birth", "Date of Death"];
                        echo "<table class=\"table\"><tr>";
                        foreach ($colheaders as $header) {
                            echo "<th>" . $header . "</th>";
                        }
                        echo "</tr>";

                        while (true) {
                            echo "<tr>";
                            foreach ($coldbnames as $name) {
                                if (is_null($row[$name]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td>" . $row[$name] . "</td>";
                            }
                            echo "</tr>";
                            
                            $row = $result->fetch_assoc();
                            if (!$row)
                                break;
                        }
                        echo "</table>";
                        $result->free();

                        echo "<br>";
                        $query = "SELECT * FROM MovieActor ma INNER JOIN Movie m ON ma.mid = m.id WHERE ma.aid=" . $actorID;
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }

                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h4>This actor did not act in any of the movies in the database!</h4>";
                            exit(1);
                        }
                        
                        echo "<h4>Filmography</h4>";
                        echo "<table class=\"table\"><tr>";
                        echo "<th>Movie</th><th>Role</th>";
                        echo "</tr>";

                        while (true) {
                            echo "<tr>";
                            if (is_null($row["title"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td><a href=\"browse_movie.php?movieID=" . $row["mid"] . "\">" . $row["title"] . "</a></td>";
                            if (is_null($row["role"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["role"] . "</td>";
                            echo "</tr>";
                            
                            $row = $result->fetch_assoc();
                            if (!$row)
                                break;
                        }
                        echo "</table>";
                        $result->free();
                    }
                    else {
                        print("<h3>No actor specified.</h3>");
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