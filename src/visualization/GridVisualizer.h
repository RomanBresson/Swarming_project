#ifndef SWARMING_PROJECT_GRIDVISUALIZER_H
#define SWARMING_PROJECT_GRIDVISUALIZER_H

#include <vtkSmartPointer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>

#include <array>
#include <vector>
#include <cmath>

#include "definitions/types.h"
#include "definitions/graphical_constants.h"
#include "data_structures/Grid.h"
#include "data_structures/Boid.h"
#include "visualization/vtkTimerCallback.h"

using types::Position;

/**
 * Visualizer for the Grid class.
 *
 * @tparam Distribution probability distribution used for the boids in the grid we want to visualize.
 * @tparam Dimension    dimension of the space represented by the grid we want to visualize.
 */
template <typename Distribution, std::size_t Dimension>
class GridVisualizer {

    static_assert(Dimension == 2 || Dimension == 3, "The Dimension of the Visualizer can only be 2 or 3.");

    using TimerCallback = vtkTimerCallback<Distribution, Dimension>;

public:

    /**
     * Construct an instance of Visualizer.
     *
     * The grid must be given as a non-const reference because we will update it directly
     * in the event-loop of VTK.
     *
     * @param grid a non-constant reference on the grid we want to visualize.
     */
    explicit GridVisualizer(Grid<Distribution, Dimension> & grid)
            : m_grid(grid),
              m_boids_actors(),
              m_renderer(vtkSmartPointer<vtkRenderer>::New()),
              m_render_window(vtkSmartPointer<vtkRenderWindow>::New()),
              m_render_window_interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New()) {

        initialize_mesh();
        initialize_boids();

        m_renderer->SetBackground(gconst::BACKGROUND_COLOR);

        m_render_window->AddRenderer(m_renderer);

        m_render_window_interactor->SetRenderWindow(m_render_window);
        m_render_window_interactor->Initialize();
        // Sign up to receive TimerEvent
        vtkSmartPointer<TimerCallback> callback = TimerCallback::New(m_grid, m_renderer, m_boids_actors);
        m_render_window_interactor->AddObserver(vtkCommand::TimerEvent, callback);
        // Create the TimerEvent
        m_render_window_interactor->CreateRepeatingTimer(gconst::UPDATE_DELAY_MS);
    }

    /**
     * Launch the visualization.
     *
     * Calls vtkRenderWindowInteractor::Start and start the event-loop of VTK.
     */
    void start() {
        // Launch the visualization
        m_render_window_interactor->Start();
    }

private:

    void initialize_mesh() {

        // If we can't represent 2^Dimension as an unsigned long long then abort compilation
        static_assert(Dimension < sizeof(unsigned long long), "The chosen Dimension is too high.");

        using point_type = std::array<bool, Dimension>;
        std::array<point_type, (1ULL << Dimension)> points;

        // For each possibility of {0,1}^Dimension (integer binary representation)
        for (std::size_t i{0}; i < (1ULL << Dimension); ++i) {
            // We transform the current integer representation as a point_type
            point_type current_point;
            for (std::size_t coordinate{0}; coordinate < Dimension; ++coordinate) {
                // Is the bit n° coordinate set to 1 or not?
                current_point[coordinate] = static_cast<bool>(i & (1LL << coordinate));
            }
            points[i] = current_point;
        }

        // Now points contains all the possibilities for {0,1}^Dimension
        // We need to draw all the pairs of points that only have 1 different coordinate.
        for (std::size_t i{0}; i < points.size(); ++i) {
            for (std::size_t j{0}; j < points.size(); ++j) {
                std::size_t number_of_differences{0};
                for (std::size_t d{0}; d < Dimension; ++d) {
                    number_of_differences += (points[i][d] != points[j][d]);
                }

                if (number_of_differences == 1) {
                    // Then draw the line
                    double point1[gconst::VTK_COORDINATES_NUMBER] = {0.0};
                    double point2[gconst::VTK_COORDINATES_NUMBER] = {0.0};

                    for (std::size_t d{0}; d < Dimension; ++d) {
                        point1[d] = static_cast<double>(points[i][d] ? m_grid.m_bottom_left[d] : m_grid.m_top_right[d]);
                        point2[d] = static_cast<double>(points[j][d] ? m_grid.m_bottom_left[d] : m_grid.m_top_right[d]);
                    }

                    vtkSmartPointer<vtkLineSource> line_source = vtkSmartPointer<vtkLineSource>::New();
                    line_source->SetPoint1(point1);
                    line_source->SetPoint2(point2);
                    line_source->Update();

                    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                    mapper->SetInputConnection(line_source->GetOutputPort());
                    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
                    actor->SetMapper(mapper);

                    m_renderer->AddActor(actor);
                }
            }
        }
    }



    /**
     * Initialize the data structure for plotting the boids.
     */
    void initialize_boids() {
        m_boids_actors.reserve(m_grid.m_boids.size());
        for(const auto & boid : m_grid.m_boids) {
            create_boid_for_rendering(boid);
        }
        std::cout << "Created " << m_grid.m_boids.size() << " boids." << std::endl;
    }

    /**
     * Create all the data structures needed to visualize one boid.
     *
     * For the moment, the visualization of a boid is only a circle. We can improve
     * this by representing a boid as a triangle and take into account the current
     * orientation of the boid.
     *
     * @param boid boid we want to add to the visualization.
     */
    void create_boid_for_rendering(const Boid<Distribution, Dimension> &boid) {
        double buffer[gconst::VTK_COORDINATES_NUMBER];

        // Compute the center of the circle
        for(std::size_t i{0}; i < Dimension; ++i) {
            buffer[i] = static_cast<double>(boid.m_position[i]);
        }

        // Create a circle
        vtkSmartPointer<vtkSphereSource> circle = vtkSmartPointer<vtkSphereSource>::New();
        circle->SetRadius(gconst::BOID_RADIUS_COEFFICIENT * m_grid.m_top_right[0]);

        // Create the mapper and the actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(circle->GetOutputPort());;

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetPosition(buffer);
        actor->GetProperty()->SetColor(gconst::BOID_COLOR);

        // Add the created actor to the renderer
        m_renderer->AddActor(actor);
        m_boids_actors.push_back(actor);

    }

    Grid<Distribution, Dimension> & m_grid;

    std::vector<vtkSmartPointer<vtkActor> > m_boids_actors;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_render_window;
    vtkSmartPointer<vtkRenderWindowInteractor> m_render_window_interactor;
};


#endif //SWARMING_PROJECT_GRIDVISUALIZER_H
