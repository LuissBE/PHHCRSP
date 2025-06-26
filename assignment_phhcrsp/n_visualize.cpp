#include "Data.h"
constexpr double PI = 3.141592653589793;
constexpr float activeConst = 1.3f;
const float outlineThickness = 2.f;
const std::vector<sf::Color> colors = {
    sf::Color(255, 255, 85),      // Yellow         0
    sf::Color(255, 153, 63),     // Orange          1
    sf::Color(255, 33, 33),    // Red               2
    sf::Color(171, 22, 100),    // Deep Purple      3
	sf::Color(253, 49, 253),    // Light Pink       4
    sf::Color(128, 152, 53),    // Kaki             5
    sf::Color(59, 52, 255),   // Blue               6
    sf::Color(140, 40, 200),   // Blue Violet       7
    sf::Color(53, 253, 0),   // Light Green         8 
    sf::Color(10, 125, 43),    // Dark Green        9
    sf::Color(140, 150, 200)     // Light Blue      10
};
const sf::Color mutedGray(80, 80, 80);

std::tuple<sf::VertexArray, sf::ConvexShape> n_drawBezierArrow(sf::Color arrowColor, sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, int numPoints = 20, int arrowSize = 10.f) {
    sf::VertexArray curve(sf::PrimitiveType::LineStrip, numPoints);
    sf::VertexArray curvethick(sf::PrimitiveType::LineStrip, numPoints);

    // Generate points along the curve using the quadratic Bézier formula
    for (int i = 0; i < numPoints; ++i) {
        float t = static_cast<float>(i) / (numPoints - 1);
        float x = (1 - t) * (1 - t) * p0.x + 2 * (1 - t) * t * p1.x + t * t * p2.x;
        float y = (1 - t) * (1 - t) * p0.y + 2 * (1 - t) * t * p1.y + t * t * p2.y;
        curve[i].position = sf::Vector2f(x, y);
        curve[i].color = arrowColor;
        curvethick[i].position = sf::Vector2f(x, y) + sf::Vector2f(1.f, 1.f);
        curvethick[i].color = arrowColor;

    }


    // Calculate the middle point (t = 0.5) of the Bézier curve
    float t_middle = 0.5f;
    float x_middle = (1 - t_middle) * (1 - t_middle) * p0.x + 2 * (1 - t_middle) * t_middle * p1.x + t_middle * t_middle * p2.x;
    float y_middle = (1 - t_middle) * (1 - t_middle) * p0.y + 2 * (1 - t_middle) * t_middle * p1.y + t_middle * t_middle * p2.y;

    // Calculate the direction vector at the middle point
    float t_offset = -0.01f; // Small offset for derivative approximation
    float x_derivative = (1 - (t_middle + t_offset)) * (1 - (t_middle + t_offset)) * p0.x + 2 * (1 - (t_middle + t_offset)) * (t_middle + t_offset) * p1.x + (t_middle + t_offset) * (t_middle + t_offset) * p2.x;
    float y_derivative = (1 - (t_middle + t_offset)) * (1 - (t_middle + t_offset)) * p0.y + 2 * (1 - (t_middle + t_offset)) * (t_middle + t_offset) * p1.y + (t_middle + t_offset) * (t_middle + t_offset) * p2.y;

    // Direction of the arrow
    sf::Vector2f direction(x_derivative - x_middle, y_derivative - y_middle);
    float angle = std::atan2(direction.y, direction.x); // angle for the direction

    // Arrowhead geometry (two lines forming the arrowhead)
    sf::Vector2f arrowLeft(arrowSize * std::cos(angle - PI / 6), arrowSize * std::sin(angle - PI / 6));  // Left part
    sf::Vector2f arrowRight(arrowSize * std::cos(angle + PI / 6), arrowSize * std::sin(angle + PI / 6)); // Right part
    sf::Vector2f arrowBase(1 * arrowSize * std::cos(angle), 1 * arrowSize * std::sin(angle));

    // Create the arrowhead (two lines)
    sf::ConvexShape arrow;
    arrow.setPointCount(4);
    arrow.setPoint(0, sf::Vector2f(x_middle, y_middle));
    arrow.setPoint(1, sf::Vector2f(x_middle, y_middle) + arrowLeft);
    arrow.setPoint(2, sf::Vector2f(x_middle, y_middle) + arrowBase);
    arrow.setPoint(3, sf::Vector2f(x_middle, y_middle) + arrowRight);
    arrow.setFillColor(arrowColor);

    return { curve,  arrow };

}

void Data::n_update_colors(std::vector<std::unique_ptr<sf::Shape>>& job_shapes, vector<vector<vector<bool>>> assignment, int d, int SQUARE_SIZE)
{

    float SHAPE_SIZE = activeConst * SQUARE_SIZE;
    for (int k = 1; k < n_jobs; k++)
    {
        int nurse_index = -1;
        for (int w = 0; w < n_nurses; w++)
        {
            if (assignment[d][w][k])
            {
                nurse_index = w;
                break;
            }
        }
        sf::Color fillColor = (nurse_index != -1) ? colors[nurse_index] : mutedGray;
        std::unique_ptr<sf::Shape> shape;
        switch (nurse_index) // Choose shape by nurse ID
        {
        default: // Square
            shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(SHAPE_SIZE, SHAPE_SIZE));
            shape->setOutlineColor(sf::Color::Black);
            shape->setOutlineThickness(outlineThickness);
            break;
        case 0: // Circle
            shape = std::make_unique<sf::CircleShape>(0.5 * SHAPE_SIZE);
            shape->setOutlineColor(sf::Color::Black);
            shape->setOutlineThickness(outlineThickness);
            break;
        case 1: // Triangle
        {
            auto triangle = std::make_unique<sf::ConvexShape>(3);
            triangle->setPoint(0, { 0.f, SHAPE_SIZE });
            triangle->setPoint(1, { 0.5f * SHAPE_SIZE, .0f });
            triangle->setPoint(2, { SHAPE_SIZE, SHAPE_SIZE });
            triangle->setOutlineColor(sf::Color::Black);
            triangle->setOutlineThickness(outlineThickness);
            shape = std::move(triangle);
            break;
        }
        case 2: // Diamond
        {
            auto diamond = std::make_unique<sf::ConvexShape>(4);
            diamond->setPoint(0, { 0.5f * SHAPE_SIZE, 0.f });
            diamond->setPoint(1, { 0.f, 0.5f * SHAPE_SIZE });
            diamond->setPoint(2, { 0.5f * SHAPE_SIZE, SHAPE_SIZE });
            diamond->setPoint(3, { SHAPE_SIZE, 0.5f * SHAPE_SIZE });
            diamond->setOutlineColor(sf::Color::Black);
            diamond->setOutlineThickness(1.f);
            diamond->setOutlineColor(sf::Color::Black);
            diamond->setOutlineThickness(outlineThickness);
            shape = std::move(diamond);
            break;
        }
        case 3: // Pentagon
        {
            auto pentagon = std::make_unique<sf::ConvexShape>(5);
            pentagon->setPoint(0, { 0.5f * SHAPE_SIZE, 0.f });
            pentagon->setPoint(1, { SHAPE_SIZE, 0.5f * SHAPE_SIZE });
            pentagon->setPoint(2, { 0.75f * SHAPE_SIZE, SHAPE_SIZE });
            pentagon->setPoint(3, { 0.25f * SHAPE_SIZE, SHAPE_SIZE });
            pentagon->setPoint(4, { 0.f, 0.5f * SHAPE_SIZE });
            pentagon->setOutlineColor(sf::Color::Black);
            pentagon->setOutlineThickness(outlineThickness);
            shape = std::move(pentagon);
            break;
        }
        case 4:
        {
            auto diamond = std::make_unique<sf::ConvexShape>(4);
            diamond->setPoint(0, { 0.5f * SHAPE_SIZE, 0.f });
            diamond->setPoint(1, { 0.f, 0.5f * SHAPE_SIZE });
            diamond->setPoint(2, { 0.5f * SHAPE_SIZE, SHAPE_SIZE });
            diamond->setPoint(3, { SHAPE_SIZE, 0.5f * SHAPE_SIZE });
            diamond->setOutlineColor(sf::Color::Black);
            diamond->setOutlineThickness(1.f);
            diamond->setOutlineColor(sf::Color::Black);
            diamond->setOutlineThickness(outlineThickness);
            shape = std::move(diamond);
            break;
        }
        case -1:
            shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        }
        shape->setFillColor(fillColor);
        shape->setPosition(job_shapes[k]->getPosition());
        job_shapes[k] = std::move(shape); // Store the shape
    }
}

void Data::n_update_arrows(std::vector<std::unique_ptr<sf::Shape>>& job_shapes, int SQUARE_SIZE, vector<vector<sf::VertexArray>>& curves, vector<vector<sf::VertexArray>>& curvesThickX, vector<vector<sf::VertexArray>>& curvesThickY, vector<vector<sf::ConvexShape>>& arrows, vector<vector<vector<vector<bool>>>> route, int d)
{
    int i, j, w;
    curves = vector<vector<sf::VertexArray>>(n_jobs, vector<sf::VertexArray>(n_jobs));
	curvesThickX = vector<vector<sf::VertexArray>>(n_jobs, vector<sf::VertexArray>(n_jobs));
	curvesThickY = vector<vector<sf::VertexArray>>(n_jobs, vector<sf::VertexArray>(n_jobs));
    arrows = vector<vector<sf::ConvexShape>>(n_jobs, vector<sf::ConvexShape>(n_jobs));
    for (i = 0; i < n_jobs; i++)
    {

        for (j = 0; j < n_jobs; j++)
        {
            if (i != j)
            {
                for (w = 0; w < n_nurses; w++)
                {
                    if (route[d][w][i][j])
                    {
                        tie(curves[i][j], arrows[i][j]) = n_drawBezierArrow(colors[w], job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
                        tie(curvesThickX[i][j], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(1.f,0.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
                        tie(curvesThickX[i][j], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(0.f, 1.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[j]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
                    }
                }
            }
        }
    }
}


void Data::n_visualize_clusters2(vector<vector<vector<bool>>> assignment, vector<vector<vector<vector<bool>>>> route)
{
    const int WINDOW_LENGTH = 1200;
    const int WINDOW_HEIGHT = 1000;
    const int MARGIN = 100;
    const int SQUARE_SIZE = 10;
    const int padding = 15; // padding between squares
    double ellipseA = WINDOW_LENGTH * 0.5 - MARGIN;
    double ellipseB = WINDOW_HEIGHT * 0.5 - MARGIN;
    bool thickFlag = true;
    sf::Vector2u Aspect_Ratio = { WINDOW_LENGTH, WINDOW_HEIGHT };
    sf::RenderWindow window(sf::VideoMode(Aspect_Ratio), "Day 0", sf::Style::Titlebar | sf::Style::Close);

    std::vector<std::unique_ptr<sf::Shape>> job_shapes(n_jobs);
    std::vector<sf::RectangleShape> cluster_squares(n_clusters);
    std::vector<std::vector<sf::VertexArray>> curves(n_jobs, std::vector<sf::VertexArray>(n_jobs));
    std::vector<std::vector<sf::VertexArray>> curvesThickX(n_jobs, std::vector<sf::VertexArray>(n_jobs));
    std::vector<std::vector<sf::VertexArray>> curvesThickY(n_jobs, std::vector<sf::VertexArray>(n_jobs));
    std::vector<std::vector<sf::ConvexShape>> arrows(n_jobs, std::vector<sf::ConvexShape>(n_jobs));
    vector<int> jobs_in_cluster;
    int clusterwidth;
    float clusterX, clusterY;

    int c, d, D, w, i, j;
    D = 0;
    //Draw the clusters and jobs
    for (c = 0; c < n_clusters; ++c)
    {
        clusterX = static_cast<float>(WINDOW_LENGTH * 0.5 + ellipseA * cos(2 * PI * c / n_clusters));
        clusterY = static_cast<float>(WINDOW_HEIGHT * 0.5 + ellipseB * sin(2 * PI * c / n_clusters));
        jobs_in_cluster.clear();
        for (j = 0; j < n_jobs; j++)
        {
            if (job[j].cluster == c)
                jobs_in_cluster.push_back(j);
        }
        clusterwidth = (int)(sqrt(jobs_in_cluster.size())) + 1;
        //cluster_squares[c] = sf::RectangleShape(sf::Vector2f(clusterwidth * (SQUARE_SIZE + padding), floor(jobs_in_cluster.size()/clusterwidth) * (SQUARE_SIZE + padding)));
        //cluster_squares[c].setFillColor(sf::Color(160, 160, 160));
        //cluster_squares[c].setPosition(sf::Vector2f(clusterX - 0.5 * clusterwidth * (SQUARE_SIZE + padding), clusterY - 0.5 * clusterwidth * (SQUARE_SIZE + padding)));
        for (int k = 0; k < jobs_in_cluster.size(); k++)
        {
            if (k == 0)
            {
                job_shapes[jobs_in_cluster[k]] = std::make_unique<sf::RectangleShape>(sf::Vector2f(activeConst * SQUARE_SIZE * 1.4f, activeConst * SQUARE_SIZE * 1.4f));
                job_shapes[jobs_in_cluster[k]]->setFillColor(sf::Color::Black);
            }
            else
            {
                job_shapes[jobs_in_cluster[k]] = std::make_unique<sf::RectangleShape>(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                job_shapes[jobs_in_cluster[k]]->setFillColor(mutedGray);
            }
            job_shapes[jobs_in_cluster[k]]->setPosition(sf::Vector2f((k % clusterwidth) * (SQUARE_SIZE + padding) + clusterX - 0.5 * clusterwidth * (SQUARE_SIZE + padding), floor(k / clusterwidth) * (SQUARE_SIZE + padding) + clusterY - 0.5 * clusterwidth * (SQUARE_SIZE + padding)));
        }
    }
    n_update_colors(job_shapes, assignment, 0, SQUARE_SIZE);
    n_update_arrows(job_shapes, SQUARE_SIZE, curves, curvesThickX, curvesThickY,arrows, route, 0);
    int selectedJob = -1;  // Pointer to selected job
    int selectedJob2 = -1;  // Pointer to selected job
    bool arrowfound = false;
    bool screenshotflag = false;
    std::unique_ptr<sf::Shape>* selectedShape = nullptr;  // Pointer to selected shape
    sf::ConvexShape* selectedArrow = nullptr;  // Pointer to selected arrow
    sf::Vector2f offset;

    while (window.isOpen())
    {
        // Use std::optional for event handling
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            // Window closed: exit
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                break;
            }

            // Window size changed: adjust view appropriately
            if (const auto* resized = event->getIf<sf::Event::Resized>())
                window.setView(sf::View(sf::FloatRect({}, sf::Vector2f(resized->size))));

            // Change day
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                {
                    D++;
                    d = D % 5;
                    window.setTitle("Day " + to_string(d) + "\t Ctrl + S to save");
                    n_update_colors(job_shapes, assignment, d, SQUARE_SIZE);
                    n_update_arrows(job_shapes, SQUARE_SIZE, curves, curvesThickX, curvesThickY, arrows, route, d);

                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                {
                    D--;
                    if (D == -1)
                        D = 4;
                    d = D % 5;
                    window.setTitle("Day " + to_string(d) + "\t Ctrl + S to save");
                    n_update_colors(job_shapes, assignment, d, SQUARE_SIZE);
                    n_update_arrows(job_shapes, SQUARE_SIZE, curves, curvesThickX, curvesThickY, arrows, route, d);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                {
                    screenshotflag = true;
                }
            }
            // Mouse pressed (left button)
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                    std::cout << "Left mouse button is pressed!" << std::endl;
                sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

                // Find which rectangle was clicked
                for (j = 0; j < n_jobs; j++) {
                    std::unique_ptr<sf::Shape>& rect = job_shapes[j];
                    if (rect->getGlobalBounds().contains(mousePos)) {
                        selectedShape = &rect;
                        offset = rect->getPosition() - mousePos;
                        selectedJob = j;
                        break; // Stop checking once we find a selected shape
                    }
                }
                // Find out which arrow was clicked
                arrowfound = false;
                for (i = 0; i < n_jobs; i++) {
                    if (arrowfound)
                        break;
                    for (j = 0; j < n_jobs; j++) {
                        if (i != j) {
                            sf::ConvexShape& arrow = arrows[i][j];
                            if (arrow.getGlobalBounds().contains(mousePos)) {
                                selectedArrow = &arrow;
                                offset = arrow.getPosition() - mousePos;
                                selectedJob = i;
                                selectedJob2 = j;
                                arrowfound = true;
                                break; // Stop checking once we find a selected shape
                            }
                        }
                    }
                }
            }

            // Mouse released (stop dragging)
            if (event->is<sf::Event::MouseButtonReleased>())
            {
                selectedShape = nullptr;
                selectedArrow = nullptr;
                selectedJob = -1;
                selectedJob2 = -1;
            }
        }
        // If dragging, update the selected shape’s position
        if (selectedShape)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            (*selectedShape)->setPosition({ static_cast<float>(mousePos.x) + offset.x, static_cast<float>(mousePos.y) + offset.y });
            for (i = 0; i < n_jobs; i++)
            {
                if (i != selectedJob)
                {
                    for (w = 0; w < n_nurses; w++)
                    {
                        if (route[d][w][selectedJob][i])
                        {
                            tie(curves[selectedJob][i], arrows[selectedJob][i]) = n_drawBezierArrow(colors[w], job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
							tie(curvesThickX[selectedJob][i], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(1.f, 0.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
							tie(curvesThickY[selectedJob][i], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(0.f, 1.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
                        }
                        if (route[d][w][i][selectedJob])
                        {
                            tie(curves[i][selectedJob], arrows[i][selectedJob]) = n_drawBezierArrow(colors[w], job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
							tie(curvesThickX[i][selectedJob], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(1.f, 0.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
							tie(curvesThickY[i][selectedJob], std::ignore) = n_drawBezierArrow(colors[w], sf::Vector2f(0.f, 1.f) + job_shapes[i]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
                        }
                    }
                }
            }
        }
        // Curve arrow
        if (selectedArrow)
        {
            sf::Color arrowcolor = selectedArrow->getFillColor();
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            tie(curves[selectedJob][selectedJob2], arrows[selectedJob][selectedJob2]) = n_drawBezierArrow(arrowcolor, job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) }, job_shapes[selectedJob2]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
			tie(curvesThickX[selectedJob][selectedJob2], std::ignore) = n_drawBezierArrow(arrowcolor, sf::Vector2f(1.f, 0.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(1.f, 0.f) + sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)), sf::Vector2f(1.f, 0.f) + job_shapes[selectedJob2]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
			tie(curvesThickY[selectedJob][selectedJob2], std::ignore) = n_drawBezierArrow(arrowcolor, sf::Vector2f(0.f, 1.f) + job_shapes[selectedJob]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), sf::Vector2f(0.f, 1.f) + sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)), sf::Vector2f(0.f, 1.f) + job_shapes[selectedJob2]->getPosition() + sf::Vector2f(SQUARE_SIZE * 0.5, SQUARE_SIZE * 0.5), 20, 10);
        }

        // Render everything
        window.clear(sf::Color::White);
        for (i = 0; i < n_jobs; i++)
        {
            for (j = 0; j < n_jobs; j++)
            {
                window.draw(curves[i][j]);
                if (thickFlag)
                {
                    window.draw(curvesThickX[i][j]);
                    window.draw(curvesThickY[i][j]);

                }
                window.draw(arrows[i][j]);
            }
        }
        for (const auto& rect : job_shapes)
            window.draw(*rect);
        window.display();
        if (screenshotflag)
        {
            sf::Texture texture;
            texture.update(window);
            sf::Image screenshot = texture.copyToImage();
            // "route_day_" + to_string(D) + ".png"
            if (screenshot.saveToFile("route_day_" + to_string(d) + ".png")) {
                //if(false) {
                std::cout << "Screenshot saved as 'screenshot.png'\n";
            }
            else {
                std::cout << "Failed to save screenshot.\n";
            }
            screenshotflag = false;
        }

    }
}


