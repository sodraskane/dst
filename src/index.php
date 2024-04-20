<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

require '../vendor/autoload.php';
require '../config/config.php';

// Import the models
require 'models/delegate.php';
require 'models/speaker.php';

// Create the web applicatoin
$app = new \Slim\App($config);

// Get container and add dependencies
$container = $app->getContainer();
require 'dependencies.php';

// Add simple basic auth middleware
require 'simple_auth.php';
$container['auth'] = function($container) {
    $simpleAuth = new \Dst\SimpleAuth($container->users);
    return $simpleAuth;
};
$app->add(function ($request, $response, $next) {
    $authUser = $_SERVER['PHP_AUTH_USER'] ?? '';
    $authPW = $_SERVER['PHP_AUTH_PW'] ?? '';
    $this->get('auth')->authenticate($authUser, $authPW);
    $response = $next($request, $response);

    return $response;
});

/*
 * This is where the enpdoints live
 */

// Gets the main application interface
$app->get('/', function (Request $request, Response $response) {
//return print_r($app->logged_in, true);
    return $this->view->render($response, 'index.twig', [
                'logged_in' => $this->get('auth')->isLoggedIn()
            ]);
});

// The login endpoint
$app->get('/login', function (Request $request, Response $response) {
    if (!$this->get('auth')->isLoggedIn()) {
        return $response->withStatus(401)
                ->withHeader('WWW-Authenticate', 'Basic realm="API and admin"');
    }
    return $response->withRedirect('/');
});

// The add endpoint
$app->get('/add', function (Request $request, Response $response) {
    return $this->view->render($response, 'add.twig', [
                'logged_in' => $this->get('auth')->isLoggedIn()
            ]);
});

// The edit endpoint
$app->get('/edit', function (Request $request, Response $response) {
    $delegates = Delegate::orderBy('id', 'asc')->get();
 
    return $this->view->render($response, 'list.twig', [
                'logged_in' => $this->get('auth')->isLoggedIn(),
		'delegates' => $delegates
            ]);
});

// The edit endpoint
$app->get('/edit/{delegate}', function (Request $request, Response $response, $args) {
    return $this->view->render($response, 'details.twig', [
                'logged_in' => $this->get('auth')->isLoggedIn(),
                'delegate' => $args['delegate']
            ]);
});

$app->get('/api', function (Request $request, Response $response) {
    $output = "This is where we will put the API documentation";
    $response->getBody()->write($output);

    return $response;
});

// Import the API endpoints
require 'endpoints/delegates.php';
require 'endpoints/speakers.php';

$app->run();
