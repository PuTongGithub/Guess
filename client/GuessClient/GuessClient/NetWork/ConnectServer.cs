using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Windows.Threading;

namespace Guess.NetWork
{
    class ConnectServer
    {
        private int player_id;
        private Socket server_socket;
        private Socket udp_socket;

        private int connect_server_tcp(IPAddress server_address)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint server_iep = new IPEndPoint(server_address, 1107);
            try
            {
                socket.Connect(server_iep);
            }
            catch
            {
                return -1;
            }

            socket.Send(Encoding.UTF8.GetBytes(player_id.ToString()));
            server_socket = socket;
            return 0;
        }

        public int connect_server(string player_name)
        {
            udp_socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint send_iep = new IPEndPoint(IPAddress.Broadcast, 1106);
            IPEndPoint recv_iep = new IPEndPoint(IPAddress.Any, 1107);

            udp_socket.Bind(recv_iep);
            udp_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, true);
            udp_socket.SendTo(Encoding.UTF8.GetBytes(player_name), send_iep);
            
            byte[] buffer = new byte[1024];
            EndPoint ep = (EndPoint)recv_iep;
            
            if(udp_socket.Poll(3000000, SelectMode.SelectRead))
            {
                udp_socket.ReceiveFrom(buffer, ref ep);
                string message = Encoding.UTF8.GetString(buffer).TrimEnd('\u0000');
                udp_socket.Close();
                if (message == "player_full")
                {
                    return 1;
                }
                else
                {
                    player_id = int.Parse(message);
                    return connect_server_tcp((ep as IPEndPoint).Address);
                }
            }
            else
            {
                udp_socket.Close();
                return -1;
            }
        }

        public int get_player_id()
        {
            return player_id;
        }

        public Socket get_socket()
        {
            return server_socket;
        }
    }
}
