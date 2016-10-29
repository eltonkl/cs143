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
            <h1>Template</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label>Occupation:</label>
                    <label class="checkbox-inline">
                        <input type="checkbox" name="type" value="Actor">Actor
                    </label>
                    <label class="checkbox-inline">
                        <input type="checkbox" name="type" value="Director"> Director
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
                    <input type="text" class="form-control" placeholder="2000-01-01" name="dob">
                    <p class="help-block"></p>
                </div>
                <div class="form-group">
                    <label for="lastName">Date of Death</label>
                    <input type="text" class="form-control" placeholder="2016-01-01" name="dob">
                    <p class="help-block">Or blank if stil alive</p>
                </div>
                <div class="form-group">
                    <label>Gender:</label>
                    <label class="checkbox-inline">
                        <input type="checkbox" name="gender" value="Male">Male
                    </label>
                    <label class="checkbox-inline">
                        <input type="checkbox" name="gender" value="Female">Female
                    </label>
                </div>

                <button type="submit" class="btn btn-default">Submit</button>
            </form>

        </div>


        <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
        <!-- Include all compiled plugins (below), or include individual files as needed -->
        <script src="js/bootstrap.min.js"></script>
    </body>
</html>