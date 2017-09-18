<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/delegates', function (Request $request, Response $response) {
    $delegates = [
        [
            'id' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ],
        [
            'id' => '13',
            'name' => 'Håkan Kvist',
            'group' => 'Drottningstaden'
        ]
    ];

    if ($request->getHeader('Accept')[0] == 'application/json') {
        return $response
            ->withHeader('Content-Type', 'application/json')
            ->write(json_encode($delegates, JSON_UNESCAPED_UNICODE));
    } else {
        return $this->view->render($response, 'list.twig', [
            'items' => $delegates,
            'title' => 'delegates'
        ]);

        $output = "This is where we will list all the delegates";
        $response->getBody()->write($output);

        return $response;
    }
});

/*
 * Add a delegate
 */
$app->post('/api/delegates', function (Request $request, Response $response) {
    $output = "This is where we will add a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Retrieve a single delegate
 */
$app->get('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will retrieve a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a delegate
 */
$app->put('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will update a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a delegate
 */
$app->patch('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will list update/patch a delegate";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Delete a delegate
 */
$app->delete('/api/delegates/{id}', function (Request $request, Response $response) {
    $output = "This is where we will delete a delegate";
    $response->getBody()->write($output);

    return $response;
});
