#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "boost/program_options.hpp"
#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"

#include "mainwindow.h"
#include "remotedatafeeder.h" 

namespace 
{ 
	const size_t ERROR_IN_COMMAND_LINE = 1; 
	const size_t SUCCESS = 0; 
	const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
}

typedef enum {
	plot_mem 	= 0x01,
	plot_cpu	= 0x02,
	plot_max	= 3
} item_validator;


class GUIinThread : public Poco::Runnable
{
  public:
    GUIinThread(std::shared_ptr<RemoteDataFeeder> observer, int plotOtions):
    m_dataObserver(observer),
    m_plotOtions(plotOtions)
    {
    }
    
    int addPlot(std::vector<std::shared_ptr<MainWindow>> &plots, int ind, int a, PlotType plotType)
	{
		std::shared_ptr<MainWindow> newPlot(new MainWindow);
		plots.push_back( std::move(newPlot) );
		plots[ind]->setupObserver( m_dataObserver, a, plotType);
		plots[ind]->show();
		return ind+1;
	}
	
    virtual void run()
    {
      int argc = 1;
      QByteArray argvStr = QString("").toLatin1();
      char* argv = argvStr.data();
      QApplication app(argc, &argv);
	  
	  // Wait until data is available to run time series timeSeriesPlotter
	  while (!m_dataObserver->isReceivingData());

      // Setting up time series plotter with new incoming data feeds
      int processSize = m_dataObserver->data()->getProcessSize();
	  cout << " - Monitoring "<< processSize << " process(es) "<<endl; 
	  
	  // Setting up step value
	  int step = 0;
	  int plot_options = m_plotOtions;
	  int plot_value	= 0x02;
	  
	  while (plot_options > 0)
	  {
		  if (plot_options & plot_value){
			  step++;
			  plot_options = (plot_options ^ plot_value);
		  }
		  plot_value >>= 1;
	  }
	  
	  // Plotting for desired processes
	  std::vector<std::shared_ptr<MainWindow>> my_plots;
      for (int i=0, a=0; i<processSize*step; i+=step, a++)
      {
		int curr_ind = i;
		// Add memory plot
		if (m_plotOtions & plot_mem)
			curr_ind = addPlot(my_plots, curr_ind, a, kMemory);
		
		// Add cpu usage plot
		if (m_plotOtions & plot_cpu)
			curr_ind = addPlot(my_plots, curr_ind, a, kCPU);
      }
     
      app.exec();
    }
  
  private:
    std::shared_ptr<RemoteDataFeeder> 	m_dataObserver;
	int									m_plotOtions;
};

int main(int argc, char *argv[])
{
	std::string appName = argv[0]; 
	int plotOtions = 0;
	std::string endpoint ="";
	try 
	{ 
		
		namespace po = boost::program_options; 
		po::options_description desc("Options"); 
		desc.add_options() 
		("help,h", "Print help messages")
		("memory,m", "Plot total memory")  
		("cpu,c", "Plot cpu usage")
		("endpoint,n", po::value<std::string>()->required(), "tcp://server::port") ; 
		
		po::positional_options_description positionalOptions; 
		positionalOptions.add("memory", 1); 
		positionalOptions.add("cpu", 1); 
		po::variables_map vm; 
	
		try 
		{ 
		po::store(po::command_line_parser(argc, argv).options(desc) 
					.positional(positionalOptions).run(), 
					vm);
		
		if ( vm.count("help")  ) 
		{ 
			std::cout << "usage: ./proval -m -c" 
					<< std::endl << std::endl; 
			return SUCCESS; 
		} 
	
		po::notify(vm); 
		} 
		catch(boost::program_options::required_option& e) 
		{ 
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		return ERROR_IN_COMMAND_LINE; 
		} 
		catch(boost::program_options::error& e) 
		{ 
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		return ERROR_IN_COMMAND_LINE; 
		} 
		
		if ( vm.count("endpoint") ) 
		{ 
			endpoint = vm["endpoint"].as<std::string>();
		} 
		if ( vm.count("memory") ) 
		{ 
			plotOtions |= plot_mem;
		} 
		if ( vm.count("cpu") ) 
		{ 
			plotOtions |= plot_cpu;
		} 
	
	} 
	catch(std::exception& e) 
	{ 
		std::cerr << "Unhandled Exception reached the top of main: " 
				<< e.what() << ", application will now exit" << std::endl; 
		return ERROR_UNHANDLED_EXCEPTION; 
	} 
    
	// "tcp://127.0.0.1:5563";
	std::shared_ptr<RemoteDataFeeder> dataObserver( new RemoteDataFeeder(endpoint) );
	  
	GUIinThread timeseries(dataObserver, plotOtions);	
	Poco::ThreadPool::defaultPool().start(*dataObserver);
	Poco::ThreadPool::defaultPool().start(timeseries);
	Poco::ThreadPool::defaultPool().joinAll();

	return 0;
}
