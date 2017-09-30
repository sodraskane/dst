<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/delegates', function (Request $request, Response $response) {
    $delegates = Delegate::orderBy('id', 'asc')->get();
    return $response->withJson($delegates);
});

/*
 * Add a delegate
 */
$app->post('/api/delegates', function (Request $request, Response $response) {
    if (!$this->get('auth')->isLoggedIn()) {
        return $response->withStatus(401)
                ->withHeader('WWW-Authenticate', 'Basic realm="API and admin"');
    }

    $data = $request->getParsedBody();
    $delegate = new Delegate();
    $delegate->name = $data['name'];
    $delegate->group = $data['group'];

    $delegate->save();

    return $response->withJson($delegate)
            ->withStatus(201)
            ->withHeader('Location:', '/api/delegates/' + $delegate->id);
});

/*
 * Retrieve a single delegate
 */
$app->get('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $delegate = Delegate::find($args['delegate']);
    if ($delegate != null) {
        $response = $response->withJson($delegate);
    } else {
        $response = $response->withStatus(404);
    }
    return $response;
});

/*
 * Update a delegate
 */
$app->put('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    if (!$this->get('auth')->isLoggedIn()) {
        return $response->withStatus(401)
                ->withHeader('WWW-Authenticate', 'Basic realm="API and admin"');
    }

    $data = $request->getParsedBody();
    $delegate = Delegate::find($args['delegate']);
    if (!$delegate) {
        return $response->withStatus(404);
    }
    $delegate->id = $data['id'] ?: $delegate->id;
    $delegate->name = $data['name'] ?: $delegate->name;
    $delegate->group = $data['group'] ?: $delegate->group;

    $delegate->save();

    return $response->withStatus(204);
});

/*
 * Delete a delegate
 */
$app->delete('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    if (!$this->get('auth')->isLoggedIn()) {
        return $response->withStatus(401)
                ->withHeader('WWW-Authenticate', 'Basic realm="API and admin"');
    }

    $delegate = Delegate::find($args['delegate']);
    if ($delegate) {
         $delegate->delete();
    }

    return $response->withStatus(204);
});
