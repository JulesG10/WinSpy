using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using System.IO;
using System.Web;

namespace WinSpyConsole
{
    public class WebServer
    {
        private readonly HttpListener _listener = new HttpListener();
        private readonly Func<HttpListenerRequest, string> _responderMethod;

        public WebServer(IReadOnlyCollection<string> prefixes, Func<HttpListenerRequest, string> method)
        {
            foreach (var s in prefixes)
            {
                _listener.Prefixes.Add(s);
            }
            _responderMethod = method;
            _listener.Start();
        }

        public WebServer(Func<HttpListenerRequest, string> method, params string[] prefixes): this(prefixes, method)
        {
        }

        public void Run()
        {
            ThreadPool.QueueUserWorkItem(o =>
            {
                try
                {
                    while (_listener.IsListening)
                    {
                        ThreadPool.QueueUserWorkItem(c =>
                        {
                            HttpListenerContext ctx = c as HttpListenerContext;
                            try
                            {
                                if (ctx == null)
                                {
                                    return;
                                }

                                string rstr = _responderMethod(ctx.Request);
                                byte[] buf = Encoding.UTF8.GetBytes(rstr);
                                ctx.Response.ContentLength64 = buf.Length;
                                ctx.Response.OutputStream.Write(buf, 0, buf.Length);
                            }
                            catch
                            {}
                            finally
                            {
                                if (ctx != null)
                                {
                                    ctx.Response.OutputStream.Close();
                                }
                            }
                        }, _listener.GetContext());
                    }
                }
                catch (Exception){}
            });
        }

        public void Stop()
        {
            _listener.Stop();
            _listener.Close();
        }
    }



    class Program
    {
        static void Main(string[] args)
        {
            new Program();
        }

        private static string Content;
        private static int Id = 0;

        public Program()
        {
            UpdateContent();
            bool isStart = false;
            WebServer ws = new WebServer(SendResponse, "http://localhost:3000/");

            while (true)
            {
                Console.Write("$ ");
                string line = Console.ReadLine();
               
                
                if(line.Split(' ')[0] == "start")
                {
                    isStart = true;
                    try
                    {
                        if (line.Split(' ').Length > 1)
                        {
                            ws = new WebServer(SendResponse, line.Split(' ')[1]);
                        }
                        ws.Run();
                    }
                    catch 
                    {
                        ws = new WebServer(SendResponse, "http://localhost:3000/");
                        ws.Run();
                    }
                }else if(line == "stop" && isStart)
                {
                    ws.Stop();
                }
                else if(line == "exit" && isStart)
                {
                    ws.Stop();
                    break;
                }
                else
                {
                    string[] content = line.Split(' ');
                    Content = "id: " + Id.ToString() + "\n";
                    Content += "action: " + content[0].ToUpper() + "\n";

                    List<string> list = new List<string>(content);
                    list.RemoveAt(0);
                    Content += "content: " + string.Join(" ", list.ToArray());
                }
            }
        }

        private static void UpdateContent()
        {
            Id++;
            Content = "id: " + Id.ToString() + "\n";
            Content += "action: NONE\n";
            Content += "content:";
        }

        private static string GetContent(Stream body, Encoding encoding)
        {
            StreamReader reader = new System.IO.StreamReader(body, encoding);
            string s = reader.ReadToEnd();
            body.Close();
            reader.Close();
            return s;
        }

        private static string SendResponse(HttpListenerRequest request)
        {
            if (request.RawUrl != "/")
            {
                foreach (string str in request.QueryString)
                {
                    Console.WriteLine(request.QueryString[str]);
                }
            }
            
            if(!request.HasEntityBody)
            {
                string content = GetContent(request.InputStream, request.ContentEncoding);
                if (content.Trim() == "")
                {
                    content = Content;
                    UpdateContent();
                    return content;
                }
                else
                {
                    Console.WriteLine(content);
                }
            }
            return "";
        }
    }
}
