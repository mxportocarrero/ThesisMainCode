#include "../inc/utilities.hpp"

// Función para mostrar
void show_depth_image(const cv::String & win_name, const cv::Mat & depth_img){
    double min;
    double max;
    cv::minMaxIdx(depth_img, &min, &max);
    cv::Mat adjMap;
    cv::convertScaleAbs(depth_img, adjMap, 255 / max);
    cv::imshow(win_name, adjMap);
}

// Leer una imagen normal y la transforma a escala de grises [0,1]
// usamos por defecto solo doubles, para mejorar la precision
bool read_image(cv::Mat & img,const cv::String & img_name){
    // Leemos la imagen
    cv::Mat img_f = cv::imread(img_name,cv::IMREAD_COLOR);

    if( !img_f.data ){
        std::cout << "Could not open or find the image" << std::endl;
        return false;
    }

    //Transformamos los valores a double
    cv::Mat img_g;
    img_f.convertTo(img_g, CV_64FC3);
    img = cv::Mat::zeros(cv::Size(img_f.cols,img_f.rows),CV_64FC1);

    for(int y = 0; y < img.rows; y++)
        for(int x = 0; x < img.cols; x++){
            // no necesariamente cumple con el orden RGB, puede ser tambien BGR
            double b =img_g.at<cv::Vec3d>(y,x)[0];
            double g =img_g.at<cv::Vec3d>(y,x)[1];
            double r =img_g.at<cv::Vec3d>(y,x)[2];

            // Hay varias formas para calcular el grayscale
            //img.at<myNum>(y,x) = (r+g+b)/3; // segun el paper
            img.at<double>(y,x) = 0.299 * r + 0.587 * g + 0.114 * b; // segun opencv (CCIR 601)

            //Normalizamos el color para [0,1]
            img.at<double>(y,x) = img.at<double>(y,x) / 255.0;
        }
    return true;
}

// Leer una imagen de profundidad de datos en 16bit unsigned int
bool read_depth_image(cv::Mat & depth,const cv::String & img_name, const float & scalar_factor){
    // leemos los valores enteros de 16 bits
    cv::Mat depth_f = cv::imread(img_name,cv::IMREAD_ANYDEPTH);

    if( !depth_f.data ){
        std::cout << "Could not open or find the image" << std::endl;
        return false;
    }

    // Transformamos los valores a double
    depth_f.convertTo(depth, CV_64FC1);

    depth = depth / scalar_factor;

    return true;
}

void writeMat2File(const cv::Mat & M, const char* file){
    std::ofstream fout(file);
    fout.precision(4);

    if(!file){
        std::cout << "File not opened\n"; return;
    }

    FOR(j,M.rows){
        FOR(i,M.cols)
            fout << M.at<double>(j,i) << "\t";
        fout << std::endl;
    }
    fout.close();
}

void writeEigenMat2File(const Eigen::MatrixXd & Mat, const char* file){
    std::ofstream fout(file);
    fout.precision(4);

    if(fout.is_open()){
        fout << Mat ;
    }

    fout.close();
}

void writeEigenVec2File(const Eigen::VectorXd& Vec, const char* file){
    std::ofstream fout(file);
    fout.precision(4);

    if(fout.is_open())
        fout << Vec;

    fout.close();
}

// Funcion split para leer los datos

std::vector<std::string> split(const std::string& s, char separator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(separator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}