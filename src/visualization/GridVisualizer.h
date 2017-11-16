#ifndef SWARMING_PROJECT_GRIDVISUALIZER_H
#define SWARMING_PROJECT_GRIDVISUALIZER_H

#include <vtkSmartPointer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "definitions/types.h"
#include "definitions/graphical_constants.h"
#include "data_structures/Grid.h"
#include "data_structures/Boid.h"

using types::Position;

template <typename Distribution, std::size_t Dimension>
class GridVisualizer {

    static_assert(Dimension == 2 || Dimension == 3, "The Dimension of the Visualizer can only be 2 or 3.");

public:

    explicit GridVisualizer(const Grid<Distribution, Dimension> & grid)
            : m_grid(grid),
              m_buffer(),
              m_renderer(vtkSmartPointer<vtkRenderer>::New()),
              m_render_window(vtkSmartPointer<vtkRenderWindow>::New()),
              m_render_window_interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New())
    {
        m_render_window->AddRenderer(m_renderer);
        m_render_window_interactor->SetRenderWindow(m_render_window);
    }

    void render() {
        std::cout << "Drawing the boids... " << std::flush;
        for(const Boid<Distribution, Dimension> & boid : m_grid.m_boids) {
            draw_boid(boid);
        }
        std::cout << "\rBoids drawed!" << std::endl;
        std::cout << "Rendering... " << std::flush;
        m_render_window_interactor->Render();
        std::cout << "\rRendered!" << std::endl;
    }


private:

    template <typename Distribution2>
    void draw_boid(const Boid<Distribution2, Dimension> & boid)
    {
        // Compute the center of the circle
        for(std::size_t i{0}; i < Dimension; ++i) {
            m_buffer[i] = static_cast<double>(boid.m_position[i]);
        }

        // Create a circle
        vtkSmartPointer<vtkRegularPolygonSource> circle =
                vtkSmartPointer<vtkRegularPolygonSource>::New();
        //circle->GeneratePolygonOff();
        circle->SetNumberOfSides(gconst::BOID_NUMBER_OF_SIDES);
        circle->SetRadius(gconst::BOID_RADIUS);
        circle->SetCenter(m_buffer);

        // Create the mapper and the actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(circle->GetOutputPort());;

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        // Add the created actor to the renderer
        m_renderer->AddActor(actor);
    }


    const Grid<Distribution, Dimension> & m_grid;

    double m_buffer[gconst::VTK_COORDINATES_NUMBER];

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_render_window;
    vtkSmartPointer<vtkRenderWindowInteractor> m_render_window_interactor;
};


#endif //SWARMING_PROJECT_GRIDVISUALIZER_H
