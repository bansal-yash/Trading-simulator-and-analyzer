import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.utils import formatdate


def send_email(receiver_email, subject, message):
    msg = MIMEMultipart()
    msg["From"] = "cs5221133@iitd.ac.in"
    msg["To"] = receiver_email
    msg["Subject"] = subject
    msg["Date"] = formatdate(localtime=True)
    msg.attach(MIMEText(message, "plain"))
    server = smtplib.SMTP_SSL("smtp.iitd.ac.in", 465)
    server.login("cs5221133@iitd.ac.in", "")
    server.sendmail("cs5221133@iitd.ac.in", receiver_email, msg.as_string())
    server.quit()
