<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

require '../vendor/autoload.php';
require '../config/config.php';

// Create the web applicatoin
$app = new \Slim\App($config);

// Get container and add dependencies
$container = $app->getContainer();
require 'dependencies.php';

/*
 * This is where the enpdoints live
 */

// Gets the main application interface
$app->get('/', function (Request $request, Response $response) {
    return $this->view->render($response, 'index.twig', []);
});

// The login endpoint
$app->get('/login', function (Request $request, Response $response) {
    return $this->view->render($response, 'index.twig', []);
});

// The edit endpoint
$app->get('/edit', function (Request $request, Response $response) {
    return $this->view->render($response, 'index.twig', []);
});

$app->get('/api', function (Request $request, Response $response) {
    $output = "This is where we will put the API documentation";
    $response->getBody()->write($output);

    return $response;
});

// Import the two main endpoints
require 'endpoints/delegates.php';
require 'endpoints/speakers.php';

$app->run();
