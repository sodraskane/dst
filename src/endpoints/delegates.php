<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/delegates', function (Request $request, Response $response) {
    $delegates = Delegate::all();
    return $response->withJson($delegates);
});

/*
 * Add a delegate
 */
$app->post('/api/delegates', function (Request $request, Response $response) {
    $data = $request->getParsedBody();
    $delegate = new Delegate();
    $delegate->name = $data['name'];
    $delegate->group = $data['group'];

    $delegate->save();

    return $response->withJson($delegate)
            ->withStatus(201)
            ->withHeader('Location:', '/api/delegates/' + $delegate->delegate);
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
    $data = $request->getParsedBody();
    $delegate = Delegate::find($args['delegate']);
    if (!$delegate) {
        return $response->withStatus(404);
    }
    $delegate->delegate = $data['delegate'] ?: $delegate->delegate;
    $delegate->name = $data['name'] ?: $delegate->name;
    $delegate->group = $data['group'] ?: $delegate->group;

    $delegate->save();

    return $response->withStatus(204);
});

/*
 * Delete a delegate
 */
$app->delete('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $delegate = Delegate::find($args['delegate']);
    if ($delegate) {
         $delegate->delete();
    }

    return $response->withStatus(204);
});
