<html>
    <body>
        <h2>CS 143 Project 1 Part A</h2>
        <h3>Calculator</h3>

        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            Expression: <input type="text" name="fexp">
            <input type="submit" value="Calculate">
        </form>

        <?php
            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $exp = $_REQUEST['fexp'];
                
                echo "<h3>$exp</h3>";

                // TODO:
                // use preg_match() to do input validation
                // use eval() to execute the string directly
                // what else?
            }
        ?>


    </body>
</html>