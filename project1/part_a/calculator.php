<html>
    <body>
        <h2>CS 143 Project 1 Part A</h2>
        <h3>Calculator</h3>

        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            Expression: <input type="text" name="expr">
            <input type="submit" value="Calculate">
        </form>

        <?php
            // Approach:
            // use preg_match() to do input validation
            // use eval() to execute the string directly
            // what else?

            // TODO: fix REGEX
            // http://stackoverflow.com/questions/11009320/validate-mathematical-expressions-using-regular-expression
            //define("MATH_EXPRESSION_REGEX", "\d+(\.\d+)?([+-*/]\d+(\.\d+)?)*");
            // define("MATH_EXPRESSION_REGEX", "^\d+(\.\d+)?([+-*/])*$");


            define("STR_INVALID_EXPRESSION", "Invalid Expression");
            define("STR_ZERO_DIVISION_ERR", "Divided by zero error!");

            define("MATH_EXPRESSION_REGEX", "^\-?\d+(\.\d+)?([\+\-\*\/]\-?\d+(\.\d+)?)*$");
            define("INVALID_ZEROES_REGEX", "(^|[\+\-\*\/])0{2,}\.");
            define("ZERO_DIVISION_REGEX", "\/0");

            // TODO: check multiple leading 0's

            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $exp = $_REQUEST['expr'];

                if (!empty($exp)) {
                    $output = STR_INVALID_EXPRESSION;
                    $expOneSpace = preg_replace('/\s+/', ' ', $exp);
                    $expNoSpace = preg_replace('/\s+/', '', $exp);

                    if (preg_match("/" . MATH_EXPRESSION_REGEX . "/", $expNoSpace)) {
                        // valid math expression
                        if (preg_match("/" . ZERO_DIVISION_REGEX . "/", $expNoSpace)) {
                            // divided by 0 error
                            $output = STR_ZERO_DIVISION_ERR;
                        } else {
                            if (!preg_match("/" . INVALID_ZEROES_REGEX . "/", $expNoSpace)) {
                                // valid math expression without invalid leading zeroes before .
                                eval( '$output = (' . $expNoSpace . ');' );
                                $output = $expOneSpace . " = " . $output;
                            }
                        }
                    }
                    echo $output;
                }
            }
        ?>


    </body>
</html>