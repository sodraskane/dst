<?php
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;

/*
 * List all delgates
 */
$app->get('/api/speakers', function (Request $request, Response $response) {
    $speakers = [
        '3' => [
            'id' => '42',
            'name' => 'Johan Holmberg',
            'group' => 'Månstorp'
        ],
        '4' => [
            'id' => '13',
            'name' => 'Håkan Kvist',
            'group' => 'Drottningstaden'
        ]
    ];

    if ($request->getHeader('Accept')[0] == 'application/json') {
        return $response
            ->withHeader('Content-Type', 'application/json')
            ->write(json_encode($speakers, JSON_UNESCAPED_UNICODE));
    } else {
        return $this->view->render($response, 'list.twig', [
            'items' => $speakers,
            'title' => 'speakers'
        ]);

        $output = "This is where we will list all the delegates";
        $response->getBody()->write($output);

        return $response;
    }
});

/*
 * Add a speaker
 */
$app->post('/api/speakers', function (Request $request, Response $response) {
    $output = "This is where we will add a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Retrieve a single speaker
 */
$app->get('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will retrieve a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a speaker
 */
$app->put('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will update a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Update a speaker
 */
$app->patch('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will list update/patch a speaker";
    $response->getBody()->write($output);

    return $response;
});

/*
 * Delete a speaker
 */
$app->delete('/api/speakers/{id}', function (Request $request, Response $response) {
    $output = "This is where we will delete a speaker";
    $response->getBody()->write($output);

    return $response;
});
