<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/delegates', function (Request $request, Response $response) {
    $delegates = [
        [
            'delegate' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ],
        [
            'delegate' => '13',
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
    $delegate = [
            'delegate' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ];
    $response->getBody()->write(json_encode($delegate, JSON_UNESCAPED_UNICODE))
            ->withStatus(201)
            ->withHeader('Location:', '/api/delegates/42');

    return $response;
});

/*
 * Retrieve a single delegate
 */
$app->get('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $delegate = [
            'delegate' => $args['delegate'],
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ];
    $response->getBody()->write(json_encode($delegate, JSON_UNESCAPED_UNICODE));

    return $response;
});

/*
 * Update a delegate
 */
$app->put('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});

/*
 * Update a delegate
 */
$app->patch('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});

/*
 * Delete a delegate
 */
$app->delete('/api/delegates/{delegate}', function (Request $request, Response $response, $args) {
    $response->withStatus(204);

    return $response;
});
