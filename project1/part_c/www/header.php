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
        <nav class="navbar navbar-inverse navbar-fixed-top">
            <div class="container">
                <!-- Brand and toggle get grouped for better mobile display -->
                <div class="navbar-header">
                    <!-- TODO: do we need mobile support lol -->
                    <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1" aria-expanded="false">
                        <span class="sr-only">Toggle navigation</span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                    </button> <!-- end mobile support -->

                    <a class="navbar-brand" href="index.php">CS143 Project 1C</a>
                </div>

                <!-- Collect the nav links, forms, and other content for toggling -->
                <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
                    <ul class="nav navbar-nav">
                        <!-- Input pages -->
                        <li class="dropdown">
                            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">Add New Contents<span class="caret"></span></a>
                            <ul class="dropdown-menu">
                                <li><a href="add_actor_director.php">Actor/Director</a></li>
                                <li><a href="add_movie.php">Movie</a></li>
                                <li><a href="add_comment.php">Comments</a></li>
                                <li role="separator" class="divider"></li>
                                <li><a href="add_actor_movie_relation.php">Actor/Movie Relation</a></li>
                                <li><a href="add_director_movie_relation.php">Director/Movie Relation</a></li>
                            </ul>
                        </li>

                        <!-- Browsing pages -->
                        <li class="dropdown">
                            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">Browse Contents<span class="caret"></span></a>
                            <ul class="dropdown-menu">
                                <li><a href="#">Actor</a></li>
                                <li><a href="#">Movie</a></li>
                            </ul>
                        </li>
                    </ul>

                    <!-- Search-->
                    <form method="get" class="navbar-form navbar-right" action="search.php?query=$_REQUEST['query']">
                        <div class="form-group">
                            <input type="text" name="query" class="form-control" placeholder="Search">
                        </div>
                        <button type="submit" class="btn btn-primary">Submit</button>
                    </form>
                </div><!-- /.navbar-collapse -->
            </div><!-- /.container-fluid -->
        </nav>
    </body>
</html>