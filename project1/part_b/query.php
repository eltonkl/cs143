<html>
    <head>
        <title>Project 1B Web Query Interface</title>
    </head>

    <body>
        <h2>CS 143 Project 1 Part B</h2>
        <h3>Web Query Interface</h3>

        <p>Query:</p>
        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            <textarea name="query" rows=20 cols=60><?php
                if ($query = $_GET['query'])
                    echo $query;
            ?></textarea>
            <input type="submit" Value="Submit">
        </form>

        <?php
            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $query = $_REQUEST['query'];

                if (!empty($query)) {
                    $db = new mysqli('localhost', 'cs143', '', 'CS143');
                    if ($db->connect_errno > 0) {
                        die('Unable to connect to database [' . $db->connect_error . ']');
                    }

                    $result = $db->query($query);
                    
                    if (!$result) {
                        $errmsg = $db->error;
                        print "Query failed: $errmsg<br />";
                        exit(1);
                    }
                    
                    $row = $result->fetch_assoc();
                    if (!$row) {
                        echo "<h3>No results!</h3>";
                        exit(1);
                    }
                    else
                        echo "<h3>Results from MySQL:</h3>";

                    echo "<table border=1 cellspacing=1 cellpadding=2><tr>";
                    $colnames = array_keys($row);
                    foreach ($colnames as $name) {
                        echo "<th>" . $name . "</th>";
                    }
                    echo "</tr>";

                    while (true) {
                        echo "<tr>";
                        foreach ($colnames as $name) {
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
                }
            }
        ?>
    </body>
</html>
