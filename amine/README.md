server {                              
    listen 8080 ;            
    server_name example.com;          
    error_page 404 /errors/404.html;  
    client_max_body_size  10M;         
    location / {                      
    
    }                                 
    location /redirect {              
        return 301 http://google.com; 
    }                                 
    location /uploads  {              
        root /var/www/uploads;        
        upload_enable on;             
        upload_store /var/www/uploads;
        allow_methods POST;           
    }                                 
}                                     

empty locations are correct need to add this 

also need to seperate with the newlines and not ignore them

don't forget to handle comments

# TODO: resolve_config() : check Configuration()