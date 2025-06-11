
const currentUrl = new URL(window.location.href);
const ip = currentUrl.searchParams.get('127.0.0.1:8000');
console.log(window.location.href);
console.log(ip);
/*document.getElementById('togglePassword').addEventListener('click', function (e) {
  const password = document.getElementById('pwd');
  const type = password.getAttribute('type') === 'password' ? 'text' : 'password';
  const eye=document.getElementById("ojo");
  eye.classList.toggle("fa-eye-slash")
  password.setAttribute('type', type);
  
});*/
function update(){
  fetch("/espmon/api/latest")
  .then(response=>{return response.json();
  })
  .then(data=>{
    //actualizar temperatura
    document.getElementById("Temperatura").innerHTML=data.temperatura;
    document.getElementById('tempbar').style.width=data.temperatura +'%';
    document.getElementById('tempbar').style.backgroundColor='rgb('+Math.floor((255/20)*parseFloat(data.temperatura))+',200,100)';
    //actualizar humedad
    document.getElementById("Humidity").innerHTML=data.humedad;
    document.getElementById('hbar').style.width=data.humedad+'%';
    document.getElementById('hbar').style.backgroundColor='rgb(100,200,'+Math.floor(((205/100)*parseFloat(data.humedad))+50)+')';
    //actualizar presion
    document.getElementById('Pressure').innerHTML=data.presion;
    document.getElementById('pbar').style.width=(data.presion/1000)+'%';
    document.getElementById('hbar').style.backgroundColor='rgb(100,'+Math.floor(((205/100)*(parseFloat(data.humedad))/1000)-50)+',200)';
  })
}



/*document.getElementById('insesion').addEventListener('click',async function(event){
    event.preventDefault();
    const uname=document.getElementById('username').value;
    const pwd=document.getElementById('pwd').value;
    const dat=new FormData;
    dat.append('nombre_usuario', uname);
    dat.append('contraseña', pwd);

    var xhttp = new XMLHttpRequest();
    xhttp.open("post","http://"+ip+"/inicio", true);
    xhttp.onload=function(){
      if (xhttp.response === "yes"){
        document.getElementById('in').style.display='none';
        document.getElementById('act').style.display='block';
      }
    }
    xhttp.send(dat);

});
*/

setInterval(update, 5000);
