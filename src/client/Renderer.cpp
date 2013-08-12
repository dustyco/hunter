#include "Renderer.h"

Renderer::Renderer(){

}

Renderer::~Renderer(){

}

bool Renderer::init(){
	//std::cout<<"Things!"<<std::endl;

	// SET UP WINDOW ///////////////////////////////////////////////////////////////////
	std::string window_title = "Hunter";
	sf::VideoMode video_mode = sf::VideoMode::getDesktopMode();
	#ifdef SFML_SYSTEM_WINDOWS
		// Windows will literally make the window this size and
		// let it get covered by the task bar so shrink it
		// Also, when the user unmaximizes it, this is what
		// will be remembered
		video_mode.width -= 10;
		video_mode.height -= 100;
	#endif
	window.create( video_mode, window_title, sf::Style::Resize | sf::Style::Close );
	#ifdef SFML_SYSTEM_WINDOWS
		// Maximize to fill the screen
		ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);
	#endif
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(true);
	
	cam_zoom = 20;
	//std::cout<<"Things!"<<std::endl;
	
	if (stars_far.loadFromFile("stars_far.jpg")) {
		stars_far.setSmooth(true);
		stars_far.setRepeated(true);
	}
	if (stars_medium.loadFromFile("stars_medium.jpg")) {
		stars_medium.setSmooth(true);
		stars_medium.setRepeated(true);
	}
	if (stars_close.loadFromFile("stars_close.jpg")) {
		stars_close.setSmooth(true);
		stars_close.setRepeated(true);
	}
	
	//std::cout<<"Things!"<<std::endl;
	if (hull_texture.loadFromFile("block-16.png")) {
		hull_texture.setSmooth(true);
		hull_sprite.setTexture(hull_texture);
		sf::Vector2<unsigned int> size = hull_texture.getSize();
		hull_sprite.setOrigin(size.x/2, size.y/2);
		float scale = 2.0/size.x;
		hull_sprite.setScale(scale, scale);
	} else return false;
	//std::cout<<"Things!"<<std::endl;
	
/*	if (part_texture.loadFromFile("parts.png")) {
		part_texture.setSmooth(true);
		part_sprite.setTexture(part_texture);
		part_sprite.setOrigin(32, 32);
		part_sprite.setScale(2.0/64, -2.0/64);
	} else return false;
*/
	cam_rot = 0;
	cam_pos = Vec2::ORIGIN;
	return true;
}
		
void Renderer::render(Sim& data){
	
	// DRAW WORLD ////////////////////////////////////////////////////////////////////////
	window.clear();
	ClientSim_draw(window, data);
	window.display();

}
		
bool Renderer::cleanup(){

}

sf::Vector2i Renderer::getMouseScreen(){
	return sf::Mouse::getPosition(window);
}

sf::Vector2f Renderer::getMouseWorld(){
	return window.mapPixelToCoords(getMouseScreen() );
}

bool Renderer::getEvent(sf::Event& evt){
	return window.pollEvent(evt);
}

void Renderer::zoom(float factor){
	cam_zoom *= factor;
	cam_zoom = std::max(std::min(cam_zoom, 100.0f), 10.0f);
}

void Renderer::ClientSim_draw (sf::RenderTarget& target, Sim& data)
{
	// WORLD VIEW //////////////////////////////////////////////////////////////////////
	cam_pos = data.ships[CURRENT_SHIP].pos;
	// cam_rot = ships[CURRENT_SHIP].rot;
	int w = target.getSize().x;
	int h = target.getSize().y;
	float screen_aspect = float(w)/float(h);
	sf::View view;
	view.setCenter(cam_pos.x, cam_pos.y);
	view.setRotation(cam_rot*DEG_PER_RAD);
	view.setSize(screen_aspect*cam_zoom, -cam_zoom);
	target.setView(view);
	
	// DRAW BACKGROUND //////////////////////////////////////////////////////////////////
	{
		sf::RenderStates state(sf::BlendAdd);
		sf::View unit_view(sf::FloatRect(0, 1, 1, -1));
		sf::VertexArray vert(sf::Quads, 4);
		vert[0].position = sf::Vector2f(0, 0);
		vert[1].position = sf::Vector2f(1, 0);
		vert[2].position = sf::Vector2f(1, 1);
		vert[3].position = sf::Vector2f(0, 1);
		
		// Far
		Vec2 c = convert(view.getCenter());
		c *= 1.0f;
		float factor = 100.0/120;
		vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
		vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
		vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
		vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
		
		state.texture = &stars_far;
		target.setView(unit_view);
		target.draw(vert, state);
		
		// Medium
		c = convert(view.getCenter());
		c *= 2.0f;
		factor = 100.0/120;
		vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
		vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
		vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
		vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
		
		state.texture = &stars_medium;
		target.setView(unit_view);
		target.draw(vert, state);
		
		// Close
		c = convert(view.getCenter());
		c *= 4.0f;
		factor = 100.0/120;
		vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
		vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
		vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
		vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
		
		state.texture = &stars_close;
		target.setView(unit_view);
		target.draw(vert, state);
	}
	target.setView(view);
	
	// DRAW EACH SHIP //////////////////////////////////////////////////////////////////
	for (ShipMap::const_iterator ship_it=data.ships.begin(); ship_it!=data.ships.end(); ++ship_it) {
		const Ship& ship = ship_it->second;
//		part_sprite.setRotation(ship.rot*DEG_PER_RAD);
		hull_sprite.setRotation(ship.rot*DEG_PER_RAD);
		hull_sprite.setColor(sf::Color(40, 240, 40, 255));
		
		// Draw each cell
		Mat2 rot(ship.rot);
		for (int y=0; y!=CELL_STRIDE; ++y)
		for (int x=0; x!=CELL_STRIDE; ++x) {
			const Cell& cell = ship.cells[x][y];
			if (cell.there) {
				Vec2 local = Vec2(x, y)*CELL_WIDTH - ship.local_center;
				Vec2 global = rot*local + ship.pos;
				
				hull_sprite.setPosition(global.x, global.y);
				target.draw(hull_sprite);
				
/*				if (cell.part) {
					part_sprite.setPosition(global.x, global.y);
					part_sprite.setTextureRect(partTexture(cell.part->id, 64));
					target.draw(part_sprite);
				}
*/			}
		}
	}
}

sf::IntRect Renderer::partTexture (PartID id, int size)
{
	PartID row = id / SHIP_PART_STRIDE;
	PartID col = id % SHIP_PART_STRIDE;
//	cout << id << ", " << col << ", " << row << endl;
	return sf::IntRect(col*size, row*size, size, size);
}
