<html>
    <body>
        <h2>CS 143 Project 1 Part A</h2>
        <h3>Calculator</h3>

        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            Expression: <input type="text" name="fexp">
            <input type="submit" value="Calculate">
        </form>

        <?php
            // Approach:
            // use preg_match() to do input validation
            // use eval() to execute the string directly
            // what else?

            // TODO: fix REGEX
            // http://stackoverflow.com/questions/11009320/validate-mathematical-expressions-using-regular-expression
            define("MATH_EXPRESSION_REGEX", "([-+/*]\d+(\.\d+)?)*");

            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $exp = $_REQUEST['fexp'];

                if (!empty($exp)) {
                    $output = "Invalid Expression";

                    if (preg_match(MATH_EXPRESSION_REGEX, $exp)) {
                        eval( '$output = (' . $exp . ');' );
                    }
                    
                    echo $output;
                }
            }
        ?>


    </body>
</html>